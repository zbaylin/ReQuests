open ReQuests;

let () = {
  ReQuests.init();
  let onResponse = (response: Response.t) =>
    switch (response) {
    | Ok(response) =>
      Printf.eprintf(
        "Successfully fetched %s:\n%s\n",
        response |> Response.request |> Request.url,
        response |> Response.body,
      )
    | Error(err) =>
      Printf.eprintf(
        "Unsuccessfully fetched %s: \"%s\"\n",
        err |> Response.Error.request |> Request.url,
        err |> Response.Error.message,
      )
    };

  ReQuests.perform(~onResponse, Request.make("https://google.com"));
  ReQuests.perform(~onResponse, Request.make("https://google.comm"));
  ReQuests.perform(
    ~onResponse,
    Request.make(
      ~headers=["Client: ReQuests"],
      "https://httpbin.org/headers",
    ),
  );

  Luv.Loop.run(~mode=`DEFAULT, ()) |> ignore;
};
