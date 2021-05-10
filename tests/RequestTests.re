module Framework =
  Rely.Make({
    let config =
      Rely.TestFrameworkConfig.initialize({
        snapshotDir: "tests/Request/__snapshots__",
        projectDir: "",
      });
  });

open ReQuests;
Framework.(
  describe("Request", ({test, _}) => {
    test("Simple request", ({expect, _}) => {
      let onResponse = response => {
        expect.result(response).toBeOk();
      };
      let request = Request.make("https://google.com");

      ReQuests.perform(~onResponse, request);

      Luv.Loop.run(~mode=`DEFAULT, ()) |> ignore;
    });

    test("Headers", ({expect, _}) => {
      let onResponse = response => {
        expect.result(response).toBeOk();

        let body = response |> Result.get_ok |> Response.body;
        let expected = ("Client", `String("ReQuests"));

        let contains =
          switch (Yojson.Safe.from_string(body)) {
          | `Assoc([("headers", `Assoc(assoc))]) =>
            List.mem(expected, assoc)
          | _ => false
          };

        expect.bool(contains).toBeTrue();
      };

      let request =
        Request.make(
          ~headers=["Client: ReQuests"],
          "https://httpbin.org/headers",
        );

      ReQuests.perform(~onResponse, request);

      Luv.Loop.run(~mode=`DEFAULT, ()) |> ignore;
    });

    test("Proxy", ({expect, _}) => {
      let onResponse = response => {
        expect.result(response).toBeOk();
      };
      let proxy = Proxy.make(~http="::1:8118", ());
      let request = Request.make(~proxy, "https://google.com");

      ReQuests.perform(~onResponse, request);

      Luv.Loop.run(~mode=`DEFAULT, ()) |> ignore;
    });

    test("Proxy HTTPS", ({expect, _}) => {
      let onResponse = response => {
        expect.result(response).toBeOk();
      };
      let proxy = Proxy.make(~https="::1:8118", ());
      let request = Request.make(~proxy, "https://google.com");

      ReQuests.perform(~onResponse, request);

      Luv.Loop.run(~mode=`DEFAULT, ()) |> ignore;
    });
  })
);
