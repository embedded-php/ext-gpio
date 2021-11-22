<?php

define('BASE_NAME', 'gpio');

try {
  // hold a reference to the script name
  $self = basename($argv[0]);

  // list of stub files
  $stubFiles = [];

  $di = new DirectoryIterator(__DIR__);
  foreach ($di as $item) {
    // ensure that the current item is a file
    if ($item->isFile() === false) {
      continue;
    }

    // ensure that the current item is not this script
    if ($item->getFilename() === $self) {
      continue;
    }

    // ensure that only php files are being processed
    if (substr($item->getFilename(), -4) !== '.php') {
      continue;
    }

    $stubFiles[$item->getFilename()] = $item->getPathname();
  }

  // open a temporary file to be used during content processing
  $tmpFile = tempnam(sys_get_temp_dir(), sprintf('php%s', BASE_NAME));
  $handle = fopen($tmpFile, 'w');
  assert(
    is_resource($handle),
    new RuntimeException('Failed to open temporary file!')
  );

  // write stub header
  fileHeader($handle);

  // guarantee the final stub generated will be consistent
  ksort($stubFiles);

  // global namespace
  $namespace = '';
  foreach ($stubFiles as $fileName => $stubFile) {
    echo 'Processing ', $fileName, PHP_EOL;

    // extract stub file content
    $content = file($stubFile, FILE_IGNORE_NEW_LINES);

    // write content to temporary file
    fileContent($handle, $content, $namespace);
  }

  // close temporary file
  fclose($handle);

  // move temporary file to final stub file
  assert(
    rename($tmpFile, __DIR__ . sprintf('/../%s.stub.php', BASE_NAME)),
    new RuntimeException('Failed to write stub file!')
  );
} catch (Exception $exception) {
  echo 'Error! ', $exception->getMessage(), PHP_EOL;
}

function fileHeader($handle): void {
  fwrite($handle, '<?php');
  fwrite($handle, "\n");
  fwrite($handle, '/** @generate-function-entries */');
  fwrite($handle, "\n");
}

function fileContent($handle, array $content, string &$namespace): void {
  assert(
    $content[0] === '<?php',
    new RuntimeException('Trying to process a non-php file!')
  );

  $lineCnt = count($content);
  $clsName = '';
  $matches = [];
  for ($j = 1; $j < $lineCnt; $j++) {
    // namespace definition
    if (preg_match('/^namespace ([a-zA-Z0-9_\\-]+);$/', $content[$j], $matches)) {
      if ($namespace === $matches[1]) {
        // skip "redefinition" of the same namespace
        continue;
      }

      $namespace = $matches[1];
      echo ' > new namespace: ', $namespace, PHP_EOL;
      fwrite($handle, sprintf('namespace %s;', $namespace));
      fwrite($handle, "\n");

      continue;
    }

    // class definition
    if (preg_match('/^ *(abstract|final|) *class ([a-zA-Z0-9_]+)/', $content[$j], $matches)) {
      $clsName = $matches[2];
      echo ' > new class: ', $clsName, PHP_EOL;
      fwrite($handle, $content[$j]);
      fwrite($handle, "\n");

      continue;
    }

    // function definition that returns self
    if (strpos($content[$j], 'function') !== false && strpos($content[$j], 'self') !== false) {
      fwrite(
        $handle,
        str_replace(
          '): self {',
          sprintf('): %s {', $clsName),
          $content[$j]
        )
      );
      fwrite($handle, "\n");

      continue;
    }

    // ordinary line
    fwrite($handle, $content[$j]);
    fwrite($handle, "\n");
  }
}
