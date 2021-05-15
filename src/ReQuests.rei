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
  /**
    An abstract type that represents MIME data.
    Can be constructed with [make].
  */
  type t;

  /**
    The two types of MIME data, with optional names.
  */
  type partData =
    | Data({
        name: option(string),
        data: string,
      })
    | File({
        name: option(string),
        path: string,
      });

  /**
    The types of encoding. Note that if you want no encoding,
    pass [None] to [Request.make]
  */
  type encoding =
    | EightBit
    | SevenBit
    | Binary
    | QuotedPrintable
    | Base64;

  /**
    Constructs a MIME.t.
  */
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

  module Code: {
    module HTTP: {
      let continue: int;
      let switchingProtocol: int;
      let processing: int;
      let earlyHints: int;

      let ok: int;
      let created: int;
      let accepted: int;
      let nonAuthoritativeInformation: int;
      let noContent: int;
      let resetContent: int;
      let partialContent: int;
      let multiStatus: int;
      let alreadyReported: int;
      let imUsed: int;

      let mutlipleChoice: int;
      let movedPermanently: int;
      let found: int;
      let seeOther: int;
      let notModified: int;
      let temporaryRedirect: int;
      let permanentRedirect: int;

      let badRequest: int;
      let unauthorized: int;
      let paymentRequired: int;
      let forbidden: int;
      let notFound: int;
      let methodNotAllowed: int;
      let notAcceptable: int;
      let proxyAuthenticationRequired: int;
      let requestTimeout: int;
      let conflict: int;
      let gone: int;
      let lengthRequired: int;
      let preconditionFailed: int;
      let payploadTooLarge: int;
      let uriTooLong: int;
      let unsupportedMediaType: int;
      let rangeNotSatisfiable: int;
      let expectationFailed: int;
      let imATeapot: int;
      let misdirectedRequest: int;
      let unprocessableEntity: int;
      let locked: int;
      let failedDependency: int;
      let tooEarly: int;
      let upgradeRequired: int;
      let preconditionRequired: int;
      let tooManyRequests: int;
      let requestHeaderFieldsTooLarge: int;
      let unavailableForLegalReasons: int;

      let internalServerError: int;
      let notImplemented: int;
      let badGateway: int;
      let serviceUnavailable: int;
      let gatewayTimeout: int;
      let httpVersionNotSupported: int;
      let variantAlsoNegotiates: int;
      let insufficientStorage: int;
      let loopDetected: int;
      let notExtended: int;
      let networkAuthenticationRequired: int;
    };
  };
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
