;; Replace QtQuick version
^(import[ \t]+)QtQuick[ \t]+1\.[10][ \t]*$
\1Qt 4.7

;; Downgrade components version
^(import[ \t]+).*components[ \t].*$
\1Qt.labs.components 1.0

;; Set namespace from meego to fremantle
^(import[ \t]+).*meego[ \t].*$
\1org.maemo.fremantle 1.0

;; Set extras namespace from meego to fremantle
^(import[ \t]+).*meego.extras[ \t].*$
\1org.maemo.extras 1.0

^(import[ \t]+).*nokia.extras[ \t].*$
\1org.maemo.extras 1.0
