# glib-example

Simple test for g_main_context_invoke() API.
It is invoked by 2nd thread, but you can show the callback's tid is main thread.
And it is different with TEST1, this main loop is run with "NULL" parameter.

if parameter is NULL (g_main_loop_run(NULL, FALSE);), it is same with TEST1.
Because "NULL" means main default thread.
