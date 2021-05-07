let apply = (func: 't => unit, opt: option('t)) => {
  opt |> Option.iter(func);
  opt;
};
