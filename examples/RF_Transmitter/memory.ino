int freeRAM() {
  int value = 0;
  int result = 0;
  extern int *__brkval;
  extern int __heap_start;
  result = (int)&value - ((int)__brkval == 0 ? (int)&__heap_start : (int)__brkval);
  return result;
}

/* How many RAM was used */
String memoryUsage() {
  float free = freeRAM();
  return (String)(100.0-(free/2048.0)*100.0) + "% | Used bytes: " + (int)(2048-free) + " of 2048";
}