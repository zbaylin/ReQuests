type method = [ | `GET | `POST];

type t = {
  url: string,
  method,
};

let make = (~method=`GET, url) => {method, url};

let url = request => request.url;
