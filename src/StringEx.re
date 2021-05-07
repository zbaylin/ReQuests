let startsWith = (str: string, substr: string) =>
  String.sub(str, 0, String.length(substr)) == substr;
