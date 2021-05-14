open Curl;

type t = {
  data: partData,
  encoding: option(encoding),
  subparts: list(t),
  headers: list(string),
}
and partData =
  | Data({
      name: option(string),
      data: string,
    })
  | File({
      name: option(string),
      path: string,
    })
and encoding =
  | EightBit
  | SevenBit
  | Binary
  | QuotedPrintable
  | Base64;

let curlMIMEDataOfPartData = partData =>
  switch (partData) {
  | Data({name: Some(_) as name, data}) =>
    CURLMIME_DATA_WITH_NAME({data: String(data), name, filename: None})
  | File({name: Some(_) as filename, path}) =>
    CURLMIME_DATA_WITH_NAME({data: Curl.File(path), name: None, filename})
  | Data({name: None, data}) => CURLMIME_DATA(data)
  | File({name: None, path}) => CURLMIME_FILEDATA(path)
  };

let curlEncodingOfMaybeEncoding = maybeEncoding =>
  switch (maybeEncoding) {
  | Some(EightBit) => CURLMIME_8BIT
  | Some(SevenBit) => CURLMIME_7BIT
  | Some(Binary) => CURLMIME_BINARY
  | Some(QuotedPrintable) => CURLMIME_QUOTEDPRINTABLE
  | Some(Base64) => CURLMIME_BASE64
  | None => CURLMIME_NONE
  };

let rec curlMIMEOfT = (t: t) =>
  Curl.{
    data: t.data |> curlMIMEDataOfPartData,
    encoding: t.encoding |> curlEncodingOfMaybeEncoding,
    subparts: t.subparts |> List.map(curlMIMEOfT),
    headers: t.headers,
  };
