module Log = (val Timber.Log.withNamespace("ReQuests.Request"));

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
  timeout: int,
  caCertPath: option(string),
  verbose: bool,
  mime: list(MIME.t),
};

let defaultCACertPath = Sys.getenv_opt("REQUESTS_CACERT_PATH");

let make =
    (
      ~method=`GET,
      ~headers=?,
      ~followRedirects=false,
      ~proxy=?,
      ~timeout=0,
      ~caCertPath=?,
      ~verbose=false,
      ~mime=[],
      url,
    ) => {
  method,
  headers,
  followRedirects,
  url,
  proxy,
  timeout,
  caCertPath:
    switch (caCertPath) {
    | Some(_) as s => s
    | None => defaultCACertPath
    },
  verbose,
  mime,
};

// START: Getters
let url = request => request.url;
// END: Getters

let debugFunc = (handle, debugType, data) =>
  switch (debugType) {
  | Curl.DEBUGTYPE_TEXT =>
    Log.debugf(f =>
      f(
        "Request[%s, %s] TEXT: %s",
        Curl.get_private(handle),
        Curl.get_effectiveurl(handle),
        data,
      )
    )
  | Curl.DEBUGTYPE_HEADER_IN =>
    Log.debugf(f =>
      f(
        "Request[%s, %s] ⭠ HEADER IN",
        Curl.get_private(handle),
        Curl.get_effectiveurl(handle),
      )
    )
  | Curl.DEBUGTYPE_HEADER_OUT =>
    Log.debugf(f =>
      f(
        "Request[%s, %s] ⭢ HEADER OUT",
        Curl.get_private(handle),
        Curl.get_effectiveurl(handle),
      )
    )
  | Curl.DEBUGTYPE_DATA_IN =>
    Log.debugf(f =>
      f(
        "Request[%s, %s] ⭠ DATA IN",
        Curl.get_private(handle),
        Curl.get_effectiveurl(handle),
      )
    )
  | Curl.DEBUGTYPE_DATA_OUT =>
    Log.debugf(f =>
      f(
        "Request[%s, %s] ⭢ DATA OUT",
        Curl.get_private(handle),
        Curl.get_effectiveurl(handle),
      )
    )
  | Curl.DEBUGTYPE_SSL_DATA_IN =>
    Log.debugf(f =>
      f(
        "Request[%s, %s] ⭠ SSL DATA IN",
        Curl.get_private(handle),
        Curl.get_effectiveurl(handle),
      )
    )
  | Curl.DEBUGTYPE_SSL_DATA_OUT =>
    Log.debugf(f =>
      f(
        "Request[%s, %s] ⭢ SSL DATA OUT",
        Curl.get_private(handle),
        Curl.get_effectiveurl(handle),
      )
    )
  | Curl.DEBUGTYPE_END =>
    Log.debugf(f =>
      f(
        "Request[%s, %s] 🛑 END",
        Curl.get_private(handle),
        Curl.get_effectiveurl(handle),
      )
    )
  };

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

    // Timeout
    Curl.set_timeout(handle, request.timeout);

    // CA Cert Path
    request.caCertPath |> Option.iter(Curl.set_cainfo(handle));

    // Debug
    if (request.verbose) {
      Curl.set_verbose(handle, true);
      Curl.set_debugfunction(handle, debugFunc);
    };

    // MIME
    // Setting the MIME data will override the method, so only do it if it's a POST
    if (request.method == `POST) {
      request.mime
      |> List.map(mime => mime |> MIME.curlMIMEOfT)
      |> Curl.set_mimepost(handle);
    };

    handle;
  };
