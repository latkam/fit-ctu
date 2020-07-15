- kompilace: `g++ -lssl -lcrypto main.c -o main`
- spuštění: `./main`

Poznámka: soubor s certifikátem je defaultně nastavený jako `certificate.pem`,
soubor se staženou stránkou je defaultně nastavený jako `page.html`,
soubor se jménem použité šifry je defaultně nastavený jako `cipher.txt`.

Pro změnu jména souborů stačí upravit příslušný #define.

Při prvním spuštění program nezakáže žádnou šifru, protože neví, která bude použita.
Použitou šifru si však uloží do souboru a při opětovném spuštění už ji načte a zakáže.

