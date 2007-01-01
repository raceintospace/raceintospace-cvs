// Mission Replay Data Structure

typedef struct ReplyItem {
   unsigned char Qty;         // Number of Animated Sequences
   ui16 Off[35];     // Offsets to Each animated Part
} RPLY;

enum Opponent_Status {Ahead,Equal,Behind};

typedef struct oldNews {

  long offset;
  ui16 size;
  } ONEWS;
