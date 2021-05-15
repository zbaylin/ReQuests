open ReQuests;
open Framework;

describe("Response", ({test, _}) => {
  test("Response code", ({expect, _}) => {
    let onResponse = response => {
      expect.result(response).toBeOk();

      let responseCode = response |> Result.get_ok |> Response.responseCode;
      expect.int(responseCode).toBe(Response.Code.HTTP.ok);
    };

    let request = Request.make("https://httpbin.org/anything");

    ReQuests.perform(~onResponse, request);

    Luv.Loop.run(~mode=`DEFAULT, ()) |> ignore;
  });

  test("Content type", ({expect, _}) => {
    let onResponse = response => {
      expect.result(response).toBeOk();

      let contentType = response |> Result.get_ok |> Response.contentType;
      expect.string(contentType).toEqual("application/json");
    };

    let request = Request.make("https://httpbin.org/anything");

    ReQuests.perform(~onResponse, request);

    Luv.Loop.run(~mode=`DEFAULT, ()) |> ignore;
  });

  test("Headers", ({expect, _}) => {
    let onResponse = response => {
      expect.result(response).toBeOk();

      let headers = response |> Result.get_ok |> Response.headers;
      let equals = (header, _) => {
        let substr = String.sub(header, 0, 4);
        substr == "date";
      };
      expect.list(headers).toContainEqual(~equals, "date");
    };

    let request = Request.make("https://httpbin.org/anything");

    ReQuests.perform(~onResponse, request);

    Luv.Loop.run(~mode=`DEFAULT, ()) |> ignore;
  });

  test("Download speed", ({expect, _}) => {
    let onResponse = response => {
      expect.result(response).toBeOk();

      let downloadSpeed = response |> Result.get_ok |> Response.downloadSpeed;
      expect.bool(downloadSpeed > 0.).toBeTrue();
    };

    let request = Request.make("https://httpbin.org/anything");

    ReQuests.perform(~onResponse, request);

    Luv.Loop.run(~mode=`DEFAULT, ()) |> ignore;
  });
});
