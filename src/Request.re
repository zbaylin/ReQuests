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
  proxy: option(Proxy.t),
};

let make = (~method=`GET, ~headers=?, ~followRedirects=false, ~proxy=?, url) => {
  method,
  headers,
  followRedirects,
  url,
  proxy,
};

let url = request => request.url;

let makeCurlHandle: t => Curl.t =
  request => {
    let handle = Curl.init();

    // URL
    Curl.set_url(handle, request.url);
    let isHTTPS = StringEx.startsWith(request.url, "https://");

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

    // Proxy
    request.proxy
    |> Option.iter(proxy => {
         switch (proxy) {
         | Proxy.{https: Some(url), _} when isHTTPS =>
           Curl.set_proxy(handle, url) // Only use the HTTPS proxy if the URL contains it
         | Proxy.{http: Some(url), _} => Curl.set_proxy(handle, url)
         | _ => ()
         };

         Curl.set_sslverifypeer(handle, proxy.strictSSL);
       });

    handle;
  };
