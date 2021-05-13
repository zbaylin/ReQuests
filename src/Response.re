type response = {
  request: Request.t,
  curlHandle: Curl.t,
  body: string,
  headers: list(string),
};

module Error = {
  type t = {
    request: Request.t,
    curlHandle: Curl.t,
    curlCode: Curl.curlCode,
  };

  let request = error => error.request;
  let message = error => Curl.strerror(error.curlCode);
};

type t = Result.t(response, Error.t);

let request = response => response.request;
let body = response => response.body;
let headers = response => response.headers;
let responseCode = response => response.curlHandle |> Curl.get_responsecode;
let contentType = response => response.curlHandle |> Curl.get_contenttype;
let uploadSpeed = response => response.curlHandle |> Curl.get_speedupload;
let downloadSpeed = response => response.curlHandle |> Curl.get_speeddownload;
