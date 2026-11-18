// Runtime loading for assets.pak / game.pak.
//
// The paks are intentionally NOT baked into Sandbox.data. They are fetched
// from the directory that serves Sandbox.html and copied into MEMFS before
// main() runs, which means a new pak can be dropped next to the HTML and
// picked up with a plain page refresh -- no relink or rebuild required.
//
// `no-cache` still allows the browser to reuse a cached copy after
// revalidating it, so unchanged paks stay cheap and changed ones are fetched.
(function () {
  function preloadPak(name, optional) {
    var dependency = 'pak:' + name;
    addRunDependency(dependency);

    fetch(name, { cache: 'no-cache' })
      .then(function (response) {
        if (!response.ok) {
          throw new Error(response.status + ' ' + response.statusText);
        }
        return response.arrayBuffer();
      })
      .then(function (buffer) {
        FS.writeFile('/' + name, new Uint8Array(buffer));
        console.log(
          '[paks] loaded ' + name + ' (' + buffer.byteLength + ' bytes)',
        );
      })
      .catch(function (error) {
        if (optional) {
          console.warn(
            '[paks] ' + name + ' not available (' + error + '), continuing',
          );
        } else {
          console.error('[paks] failed to load ' + name + ': ' + error);
        }
      })
      .finally(function () {
        removeRunDependency(dependency);
      });
  }

  Module.preRun = Module.preRun || [];
  Module.preRun.push(function () {
    preloadPak('assets.pak', false);
    preloadPak('game.pak', true);
  });
})();
