let apply = (func: 'a => unit, res: Result.t('a, 'e)) => {
  res |> Result.iter(func);
  res;
};
