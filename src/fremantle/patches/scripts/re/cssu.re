;; Force QtQuick version to 1.1
^(import[ \t]+)QtQuick[ \t]+1\.0[ \t]*$
\1QtQuick 1.1

;; Replace Qt 4.7 dependeces with a QtQuick 1.1 one
^(import[ \t]+)Qt[ \t]+4\.7[ \t]*$
\1QtQuick 1.1

;; Downgrade components version
^(import[ \t]+).*components[ \t]+1\.[10][ \t]*(.*)$
\1Qt.labs.components 1.0 \2

;; Set namespace from meego to fremantle
^(import[ \t]+).*fremantle[ \t].*$
\1com.nokia.meego 1.0

;; Set extras namespace from meego to fremantle
^(import[ \t]+).*extras[ \t].*$
\1com.nokia.extras 1.0
