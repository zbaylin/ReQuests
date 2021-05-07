module Multi = Curl.Multi;
module HandleIDGenerator =
  IDGenerator.Make({});

type onComplete = Curl.curlCode => unit;
type onResponse = Response.t => unit;

let multi = Multi.create();

let socketPollHandleTable: Hashtbl.t(Unix.file_descr, Luv.Poll.t) =
  Hashtbl.create(32);

module CurlHandleHashTable =
  Hashtbl.Make({
    type t = Curl.t;
    let hash = handle => Curl.get_private(handle) |> Hashtbl.hash;
    let equal = (handle1, handle2) =>
      String.equal(Curl.get_private(handle1), Curl.get_private(handle2));
  });

let curlHandleToCallbackTable: CurlHandleHashTable.t(onComplete) =
  CurlHandleHashTable.create(32);

let rec checkInfo = () => {
  switch (Multi.remove_finished(multi)) {
  | Some((handle, code)) =>
    CurlHandleHashTable.find_opt(curlHandleToCallbackTable, handle)
    |> Option.iter(onComplete => onComplete(code));

    checkInfo();
  | None => ()
  };
};

let perform =
    (
      timer: Luv.Timer.t,
      fd: Unix.file_descr,
      res: Result.t(list(Luv.Poll.Event.t), Luv.Error.t),
    ) =>
  res
  |> Result.iter(events => {
       Luv.Timer.stop(timer) |> ignore;
       let flag =
         if (List.mem(`READABLE, events) && List.mem(`WRITABLE, events)) {
           Multi.EV_INOUT;
         } else if (List.mem(`READABLE, events)) {
           Multi.EV_IN;
         } else if (List.mem(`WRITABLE, events)) {
           Multi.EV_OUT;
         } else {
           Multi.EV_AUTO;
         };

       let _: int = Multi.action(multi, fd, flag);

       checkInfo();
     });

let handleSocket =
    (timer: Luv.Timer.t, fd: Unix.file_descr, action: Multi.poll) => {
  let getPollHandle = () =>
    switch (Hashtbl.find_opt(socketPollHandleTable, fd)) {
    | Some(poll) => Result.Ok(poll)
    | None =>
      let maybeSocket = Luv.Unix.Os_fd.Socket.from_unix(fd);
      Result.bind(maybeSocket, socket => Luv.Poll.init_socket(socket))
      |> ResultEx.apply(poll => {
           Hashtbl.replace(socketPollHandleTable, fd, poll)
         });
    };

  switch (action) {
  | Multi.POLL_IN =>
    getPollHandle()
    |> Result.iter(pollHandle => {
         Luv.Poll.start(pollHandle, [`READABLE], perform(timer, fd))
       })
  | Multi.POLL_OUT =>
    getPollHandle()
    |> Result.iter(pollHandle => {
         Luv.Poll.start(pollHandle, [`WRITABLE], perform(timer, fd))
       })
  | Multi.POLL_REMOVE =>
    Hashtbl.find_opt(socketPollHandleTable, fd)
    |> Option.iter(pollHandle => {
         Luv.Poll.stop(pollHandle) |> ignore;
         Hashtbl.remove(socketPollHandleTable, fd);
       })
  | _ => ()
  };
};

let onTimeout = () => {
  Multi.action_timeout(multi);
  checkInfo();
};

let createOnComplete: (Buffer.t, onResponse) => onComplete =
  (buffer, onResponse, curlCode) => {
    let response = Response.{code: curlCode, body: buffer};
    onResponse(response);
  };

let addUrl = (~onResponse=_ => (), url: string) => {
  let handle = Curl.init();
  let responseBuffer = Buffer.create(128);

  let id = HandleIDGenerator.getID() |> string_of_int;
  Curl.set_private(handle, id);

  let onComplete: onComplete = createOnComplete(responseBuffer, onResponse);

  CurlHandleHashTable.replace(curlHandleToCallbackTable, handle, onComplete);

  let writeFunc = str => {
    Buffer.add_string(responseBuffer, str);
    String.length(str);
  };

  Curl.set_writefunction(handle, writeFunc);
  Curl.set_url(handle, url);

  Multi.add(multi, handle);
  Printf.eprintf("Added URL: %s\n", url);
};

let startTimeout = (timer: Luv.Timer.t, timeoutMs: int) => {
  let timeoutMs = timeoutMs <= 0 ? 1 : timeoutMs;
  Luv.Timer.start(timer, timeoutMs, onTimeout) |> ignore;
};

let init = () =>
  Luv.Timer.init()
  |> Result.iter(timer => {
       Multi.set_socket_function(multi, handleSocket(timer));
       Multi.set_timer_function(multi, startTimeout(timer));
     });
