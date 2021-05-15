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

module Code = {
  module HTTP = {
    let continue = 100;
    let switchingProtocol = 101;
    let processing = 102;
    let earlyHints = 103;

    let ok = 200;
    let created = 201;
    let accepted = 202;
    let nonAuthoritativeInformation = 203;
    let noContent = 204;
    let resetContent = 205;
    let partialContent = 206;
    let multiStatus = 207;
    let alreadyReported = 208;
    let imUsed = 226;

    let mutlipleChoice = 300;
    let movedPermanently = 301;
    let found = 302;
    let seeOther = 303;
    let notModified = 304;
    let temporaryRedirect = 307;
    let permanentRedirect = 308;

    let badRequest = 400;
    let unauthorized = 401;
    let paymentRequired = 402;
    let forbidden = 403;
    let notFound = 404;
    let methodNotAllowed = 405;
    let notAcceptable = 406;
    let proxyAuthenticationRequired = 407;
    let requestTimeout = 408;
    let conflict = 409;
    let gone = 410;
    let lengthRequired = 411;
    let preconditionFailed = 412;
    let payploadTooLarge = 413;
    let uriTooLong = 414;
    let unsupportedMediaType = 415;
    let rangeNotSatisfiable = 416;
    let expectationFailed = 417;
    let imATeapot = 418;
    let misdirectedRequest = 421;
    let unprocessableEntity = 422;
    let locked = 423;
    let failedDependency = 424;
    let tooEarly = 425;
    let upgradeRequired = 426;
    let preconditionRequired = 428;
    let tooManyRequests = 429;
    let requestHeaderFieldsTooLarge = 431;
    let unavailableForLegalReasons = 451;

    let internalServerError = 500;
    let notImplemented = 501;
    let badGateway = 502;
    let serviceUnavailable = 503;
    let gatewayTimeout = 504;
    let httpVersionNotSupported = 505;
    let variantAlsoNegotiates = 506;
    let insufficientStorage = 507;
    let loopDetected = 508;
    let notExtended = 510;
    let networkAuthenticationRequired = 511;
  };
};
