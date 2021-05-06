module Multi = Curl.Multi;

let multi = Multi.create();

let socketPollHandleTable: Hashtbl.t(Unix.file_descr, Luv.Poll.t) =
  Hashtbl.create(32);

let rec checkInfo = () => {
  switch (Multi.remove_finished(multi)) {
  | Some((handle, _)) =>
    Printf.printf("DONE: %s\n", Curl.get_effectiveurl(handle));
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
    |> Option.iter(pollHandle => Luv.Poll.stop(pollHandle) |> ignore)
  | _ => ()
  };
};

let onTimeout = () => {
  Multi.action_timeout(multi);
  checkInfo();
};

let addUrl = (url: string) => {
  let handle = Curl.init();
  let writeFunc = str => {
    Printf.eprintf("%s", str);
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
  |> ResultEx.apply(timer => {
       Multi.set_socket_function(multi, handleSocket(timer));
       Multi.set_timer_function(multi, startTimeout(timer));
     });
