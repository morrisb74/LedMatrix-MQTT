

// Define the ChipSelect pin for the led matrix (Dont use the SS or MISO pin of your Arduino!)
// Other pins are arduino specific SPI pins (MOSI=DIN of the LEDMatrix and CLK) see https://www.arduino.cc/en/Reference/SPI
const uint8_t LEDMATRIX_CS_PIN = 15;

// Define LED Matrix dimensions (0-n) - eg: 32x8 = 31x7
const int LEDMATRIX_WIDTH = 31;  
const int LEDMATRIX_HEIGHT = 7;
const int LEDMATRIX_SEGMENTS = 4;

// The LEDMatrixDriver class instance
LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);


/**
 * This draws a sprite to the given position using the width and height supplied (usually 8x8)
 */
void drawSprite( byte* sprite, int x, int y, int width, int height )
{
  // The mask is used to get the column bit from the sprite row
  byte mask = B10000000;
  
  for( int iy = 0; iy < height; iy++ )
  {
    for( int ix = 0; ix < width; ix++ )
    {
      lmd.setPixel(x + ix, y + iy, (bool)(sprite[iy] & mask ));

      // shift the mask by one pixel to the right
      mask = mask >> 1;
    }

    // reset column mask
    mask = B10000000;
  }
}



/**
 * This function draws a string of the given length to the given position.
 */
void drawString(char* text, int len, int x, int y )
{
  for( int idx = 0; idx < len; idx ++ )
  {
    int c = text[idx] ;
    //- 32;

    // stop if char is outside visible area
    if( x + idx * 8  > LEDMATRIX_WIDTH )
      return;

    // only draw if char is visible
    if( 8 + x + idx * 8 > 0 )
      drawSprite( font8x8_basic[c], x + idx * 8, y, 8, 8 );
  }
}



void marquee_run(char* text, int ANIM_DELAY )
{
  int x=0, y=0;             // start top left
  int len = strlen(text);   // get the lenght
  int textmatrixsize = ( strlen(text) * -8 ) -1;
  for( int idx=LEDMATRIX_WIDTH ; idx > textmatrixsize; --idx )
    {
     drawString(text, len, idx, 0);
     lmd.display();
     delay(ANIM_DELAY);
     ESP.wdtFeed();
     //Serial.println("IDX=" + String(idx)); 
    }
  
}

String ConvertHTMLHex ( String Text )
{
    String HexCodeStr = "";
    
    while ( Text.indexOf("&#") > -1 and Text.charAt( Text.indexOf("&#") + 4 ) == ';' ) {
      // Special characters
      //Serial.print("It contains a specials! : ");
      HexCodeStr = Text.substring( Text.indexOf("&#") + 2 ,Text.indexOf("&#") +4 );
      //Serial.println(HexCodeStr);
      Text.replace( "&#" + HexCodeStr +";", String(char(strtol( &HexCodeStr[0], NULL, 16))));
      //DisplayText.replace( "&#80;" , String(char(0x80)) );
      }
    return Text;
}


