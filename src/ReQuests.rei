module Proxy: {
  /**
    An abstract type that represents a proxy.
    Can be constructed with [make] and used with [perform].
  */
  type t;

  /**
    [make(~http=?, ~https=?, ~strictSSL=?, ())] constructs a Proxy.t with the
    following options:
    - [http]: a proxy to be used for requests.
    - [https]: a proxy to be used to HTTPS requests.
    - [strictSSL]: tells cURL whether or not to validate the SSL cert of the proxy.
  */
  let make: (~http: string=?, ~https: string=?, ~strictSSL: bool=?, unit) => t;
};

module MIME: {
  type t;

  type partData =
    | Data({
        name: option(string),
        data: string,
      })
    | File({
        name: option(string),
        path: string,
      });
  type encoding =
    | EightBit
    | SevenBit
    | Binary
    | QuotedPrintable
    | Base64;

  let make:
    (
      ~encoding: option(encoding)=?,
      ~subparts: list(t)=?,
      ~headers: list(string)=?,
      partData
    ) =>
    t;
};

module Request: {
  /**
    The various types of HTTP/HTTPS requests.
  */
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

  /**
    An abstract type that represents a request.
    Can be constructed with [make] and sent using [perform].
  */
  type t;

  /**
    [make(~method=`GET, ~headers=["Client: ReQuests"], ~followRedirects=true, "https://google.com")]
    constructs a Request.t that, when sent, will perform a GET resut to google.com with the header
    [Client: ReQuests] and will follow redirects.
  */
  let make:
    (
      ~method: method=?,
      ~headers: list(string)=?,
      ~followRedirects: bool=?,
      ~proxy: Proxy.t=?,
      ~timeout: int=?,
      ~caCertPath: string=?,
      ~verbose: bool=?,
      ~mime: list(MIME.t)=?,
      string
    ) =>
    t;

  /**
    Get the URL of the request.
  */
  let url: t => string;
};

module Response: {
  /**
    An abstract type that represents a successful response.
  */
  type response;

  /**
    A module that contains the possible errors associated w/ responses.
  */
  module Error: {
    /**
      An abstract type that represents an errored response.
    */
    type t;

    /**
      Get the original request.
    */
    let request: t => Request.t;

    /**
      Get the error message.
    */
    let message: t => string;
  };

  /**
    The main Response type. When the request succeeds (i.e. a response is gotten),
    it will be of type [Result.Ok(response)]. If the request errors (i.e. no response),
    it will be of type [Result.Error(Error.t)].
  */
  type t = Result.t(response, Error.t);

  /**
    Get the original request.
  */
  let request: response => Request.t;

  /**
    Get the body of the response.
  */
  let body: response => string;

  /**
    Get the headers of the response.
  */
  let headers: response => list(string);

  /**
    Get the response code of the response.
  */
  let responseCode: response => int;

  /**
    Get the [content-type] of the response.
  */
  let contentType: response => string;

  /**
    Get the upload speed of the transfer
  */
  let uploadSpeed: response => float;

  /**
    Get the download speed of the transfer
  */
  let downloadSpeed: response => float;
};

/**
  Initialize the library.
  If this is not called, [perform] will essentially no-op.
*/
let init: unit => unit;

/**
  Performs a Request.t and takes an [onResponse] callback, which is called when
  the response is gotten.
*/
let perform: (~onResponse: Response.t => unit=?, Request.t) => unit;
