module Proxy: {
  type t;

  let make: (~http: string=?, ~https: string=?, ~strictSSL: bool=?, unit) => t;
};

module Request: {
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
  type t;

  let make:
    (
      ~method: method=?,
      ~headers: list(string)=?,
      ~followRedirects: bool=?,
      ~proxy: Proxy.t=?,
      ~timeout: int=?,
      string
    ) =>
    t;
  let url: t => string;
};

module Response: {
  type response;

  module Error: {
    type t;

    let request: t => Request.t;
    let message: t => string;
  };

  type t = Result.t(response, Error.t);

  let request: response => Request.t;
  let body: response => string;
  let headers: response => list(string);
  let responseCode: response => int;
  let contentType: response => string;
  let uploadSpeed: response => float;
  let downloadSpeed: response => float;
};

let init: unit => unit;

let perform: (~onResponse: Response.t => unit=?, Request.t) => unit;
