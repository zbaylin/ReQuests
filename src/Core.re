module Log = {
  include (val Timber.Log.withNamespace("ReQuests.Core"));

  let warnLuvError = (res: Result.t(_, Luv.Error.t)) =>
    switch (res) {
    | Ok(_) => ()
    | Error(err) => warnf(f => f("Luv error: %s", Luv.Error.err_name(err)))
    };
};

module Multi = Curl.Multi;
module HandleIDGenerator =
  IDGenerator.Make({});
module CurlHandleHashTable =
  Hashtbl.Make({
    type t = Curl.t;
    let hash = handle => Curl.get_private(handle) |> Hashtbl.hash;
    let equal = (handle1, handle2) =>
      String.equal(Curl.get_private(handle1), Curl.get_private(handle2));
  });

type onComplete = Curl.curlCode => unit;
type onResponse = Response.t => unit;

let multi = Multi.create();

let socketPollHandleTable: Hashtbl.t(Unix.file_descr, Luv.Poll.t) =
  Hashtbl.create(Constants.initialSocketTableSize);

let curlHandleToCallbackTable: CurlHandleHashTable.t(onComplete) =
  CurlHandleHashTable.create(Constants.initialCurlHandleTableSize);

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
       Luv.Timer.stop(timer) |> Log.warnLuvError;
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
         Luv.Poll.stop(pollHandle) |> Log.warnLuvError;
         Hashtbl.remove(socketPollHandleTable, fd);
       })
  | _ => ()
  };
};

let onTimeout = () => {
  Multi.action_timeout(multi);
  checkInfo();
};

let createOnComplete: (Request.t, Curl.t, Buffer.t, onResponse) => onComplete =
  (request', curlHandle, buffer, onResponse, curlCode) => {
    let response =
      switch (curlCode) {
      | Curl.CURLE_OK =>
        Result.Ok(
          Response.{
            request: request',
            curlHandle,
            body: Buffer.contents(buffer),
          },
        )
      | _ =>
        Result.Error(
          Response.Error.{request: request', curlHandle, curlCode},
        )
      };
    onResponse(response);
  };

let perform = (~onResponse=_ => (), request: Request.t) => {
  let handle = Request.makeCurlHandle(request);
  let responseBuffer = Buffer.create(128);

  let id = HandleIDGenerator.getID() |> string_of_int;
  Curl.set_private(handle, id);

  let onComplete: onComplete =
    createOnComplete(request, handle, responseBuffer, onResponse);
  CurlHandleHashTable.replace(curlHandleToCallbackTable, handle, onComplete);

  let writeFunc = str => {
    Buffer.add_string(responseBuffer, str);
    String.length(str);
  };

  Curl.set_writefunction(handle, writeFunc);

  Multi.add(multi, handle);
  Log.infof(f => f("Enqueued request: %s", request.url));
};

let startTimeout = (timer: Luv.Timer.t, timeoutMs: int) => {
  let timeoutMs = timeoutMs <= 0 ? 1 : timeoutMs;
  Luv.Timer.start(timer, timeoutMs, onTimeout) |> Log.warnLuvError;
};

let init = () => {
  Luv.Timer.init()
  |> ResultEx.apply(timer => {
       Multi.set_socket_function(multi, handleSocket(timer));
       Multi.set_timer_function(multi, startTimeout(timer));
     })
  |> Log.warnLuvError;
};
