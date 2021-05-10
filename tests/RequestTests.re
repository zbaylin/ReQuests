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
  describe("Request", ({test, _}) =>
    test("Simple request", ({expect, _}) => {
      let onResponse = response => {
        expect.result(response).toBeOk();
      };
      let request = Request.make("https://google.com");

      ReQuests.perform(~onResponse, request);

      Luv.Loop.run(~mode=`DEFAULT, ()) |> ignore;
    })
  )
);
