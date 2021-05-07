type method = [
  | `GET
  | `POST
  | `PUT
  | `HEAD
  | `DELETE
  | `CONNECT
  | `OPTIONS
  | `TRACE
  | `PATCH
];

type t = {
  url: string,
  method,
  headers: option(list(string)), // Optional since the set_httpheader function can override other actions
  followRedirects: bool,
};

let make = (~method=`GET, ~headers=?, ~followRedirects=false, url) => {
  method,
  headers,
  followRedirects,
  url,
};

let url = request => request.url;

let makeCurlHandle: t => Curl.t =
  request => {
    let handle = Curl.init();

    // URL
    Curl.set_url(handle, request.url);

    // HTTP method
    switch (request.method) {
    | `GET => Curl.set_httpget(handle, true)
    | `POST => Curl.set_post(handle, true)
    | `PUT => Curl.set_put(handle, true)
    | `HEAD => Curl.set_customrequest(handle, "HEAD")
    | `DELETE => Curl.set_customrequest(handle, "DELETE")
    | `CONNECT => Curl.set_customrequest(handle, "CONNECT")
    | `OPTIONS => Curl.set_customrequest(handle, "OPTIONS")
    | `TRACE => Curl.set_customrequest(handle, "TRACE")
    | `PATCH => Curl.set_customrequest(handle, "PATCH")
    };

    // HTTP header(s)
    request.headers |> Option.iter(Curl.set_httpheader(handle));

    // Follow redirects
    Curl.set_followlocation(handle, request.followRedirects);

    handle;
  };
