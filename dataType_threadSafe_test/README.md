# glib-example

It is test for GLib data type's function is thread-safe.

And also it checks performance.


-Test result

| Data Type   | Safe? - speed     | Result of data             |
| ----------- | ----------------- | -------------------------- |
| GAsyncQueue | Safe   - fastest  | ( 0% loss, 0.69 sec spend) |
| GQueue      | Unsafe - fast     | (58% loss, 0.8 sec spend)  |
| GList       | Unsafe - too slow | (98% loss, 15 sec spend)   |
| GHashTable  | N/A    - N/A      | (Can not run)              |


-Testbed

My desktop

|          |                                        |
| -------- | -------------------------------------- |
| CPU      | Intel(R) Celeron(R) CPU G530 @ 2.40GHz |
| MemTotal | 3952480 kB                             |
| OS       | Ubuntu 12.04.5 LTS                     |

