#include "fcp.h"

#define FCP_OFFSET 1
#define FCP_SIZE (256 + FCP_OFFSET)

static short fcp_cache[FCP_SIZE][FCP_SIZE];
static short fcp_next_id = 1;

void fcp_init(void) {
  for (int i = 0; i < FCP_SIZE; i++) {
    for (int j = 0; j < FCP_SIZE; j++) {
      fcp_cache[i][j] = 0;
    }
  }
  fcp_next_id = 1;
}

short fcp_get(short fg, short bg) {
  int cfg = fg + FCP_OFFSET;
  int cbg = bg + FCP_OFFSET;

  if (cfg < 0 || cfg >= FCP_SIZE || cbg < 0 || cbg >= FCP_SIZE) {
    return 0;
  }

  if (fcp_cache[cfg][cbg] != 0) {
    return fcp_cache[cfg][cbg];
  }

  if (fcp_next_id < COLOR_PAIRS) {
    init_pair(fcp_next_id, fg, bg);
    fcp_cache[cfg][cbg] = fcp_next_id;
    return fcp_next_id++;
  }

  return 0;
}
