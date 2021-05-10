ReQuests.init();

let run = args => {
  let devNull =
    if (Sys.win32) {
      Unix.openfile("nul", [], 0o640);
    } else {
      Unix.openfile("/dev/null", [], 0o640);
    };

  Unix.create_process(args[0], args, devNull, devNull, devNull);
};

// Start the proxy server using proxy.py
let pid = run([|"python", "-m", "proxy", "--port", "8118"|]);

RequestTests.Framework.cli();

// Then kill it
let _: int = run([|"kill", "-9", string_of_int(pid)|]);
