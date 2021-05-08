type t = {
  http: option(string),
  https: option(string),
  strictSSL: bool,
};

let make = (~http=?, ~https=?, ~strictSSL=true, ()) => {
  http,
  https,
  strictSSL,
};
