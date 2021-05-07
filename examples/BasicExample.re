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

  let googleRequest = Request.make("https://google.com");
  let errorRequest = Request.make("https://google.commm");

  ReQuests.perform(~onResponse, googleRequest);
  ReQuests.perform(~onResponse, errorRequest);

  Luv.Loop.run(~mode=`DEFAULT, ()) |> ignore;
};
