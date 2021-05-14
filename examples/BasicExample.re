open ReQuests;

module Log = (val Timber.Log.withNamespace("ReQuests.BasicExample"));

let () = {
  Timber.App.enable(Timber.Reporter.console(~enableColors=true, ()));
  Timber.App.setLevel(Timber.Level.debug);
  ReQuests.init();

  let onResponse = (response: Response.t) =>
    switch (response) {
    | Ok(response) =>
      Log.infof(f =>
        f(
          "Successfully fetched %s:\n%s\nHeader:%s",
          response |> Response.request |> Request.url,
          response |> Response.body,
          response |> Response.headers |> String.concat("\n"),
        )
      )
    | Error(err) =>
      Log.errorf(f =>
        f(
          "Unsuccessfully fetched %s: \"%s\"\n",
          err |> Response.Error.request |> Request.url,
          err |> Response.Error.message,
        )
      )
    };

  ReQuests.perform(~onResponse, Request.make("https://google.com"));
  ReQuests.perform(
    ~onResponse,
    Request.make("https://google.com/thisshould404"),
  );
  ReQuests.perform(
    ~onResponse,
    Request.make(
      ~headers=["Client: ReQuests"],
      ~verbose=true,
      "https://httpbin.org/headers",
    ),
  );

  Luv.Loop.run(~mode=`DEFAULT, ()) |> ignore;
};
