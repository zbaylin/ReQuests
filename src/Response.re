type response = {
  request: Request.t,
  curlHandle: Curl.t,
  body: string,
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
