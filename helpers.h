#ifndef HELPERS_H
#define HELPERS_H

String formatPriceWithComma(int price) {
  String str = String(price);
  int len = str.length();
  for (int i = len - 3; i > 0; i -= 3) {
    str = str.substring(0, i) + "," + str.substring(i);
  }
  return str;
}

#endif
