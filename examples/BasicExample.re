let () = {
  ReQuests.init();
  ReQuests.addUrl(
    ~onResponse=
      ({code, body}) =>
        Printf.eprintf(
          "Fetched google.com (%s):\n%s\n",
          Curl.strerror(code),
          Buffer.contents(body),
        ),
    "http://google.com",
  );

  Luv.Loop.run(~mode=`DEFAULT, ()) |> ignore;
};
