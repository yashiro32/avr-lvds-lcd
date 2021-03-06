/* mehPL:
 *    This is Open Source, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */








// Please see mainConfig.h!


//The order of these #includes is *very specific*
// please see the associated files (especially lcdStuff, lcdDefines, and
// lvds)


//This file contains code for *many* options, chosen by mainConfig.h
// (and possibly makefile)



#include "projInfo.h"   //Don't include in main.h 'cause that's included in other .c's?
#include "main.h"
//#include <util/delay.h> //For delay_us in pll_enable
#include <avr/pgmspace.h>
#include _TIMERCOMMON_HEADER_
//#include "../../../_commonCode/charBitmap/0.10/charBitmap.h"
#include "fbColors.h"





#include "mainConfig.h"


//This is hokey, just trying to free up space
// Frankly, these should only be included as-needed
#if (!defined(FB_QUESTION) || !FB_QUESTION)
#warning "These should only be included as-needed!"
//These two saved 800Bytes
// But that's probably just charBitmap, because sineTable is compiled via
// the makefile.
// Yeahp.
#include _SINETABLE_HEADER_
#include _CHARBITMAP_HEADER_
#endif



#include "delay_cyc.h"


//This should be handled in mainConfig, now...
#if (!defined(FB_REFRESH_ON_CHANGE) || !FB_REFRESH_ON_CHANGE)
 #define FB_REFRESH_ON_CHANGE_COUNT	TRUE
#else
 #ifndef FB_REFRESH_ON_CHANGE_COUNT
  #define FB_REFRESH_ON_CHANGE_COUNT	5
 #endif
#endif



#if (defined(ROW_SEG_BUFFER) && ROW_SEG_BUFFER)
	 #include "rowSegBuffer.h"
#endif

//a/o v70: this should be handled by writeColor.c now
//Nope, can't be removed yet...
//#define ROW_BUFFER FALSE
//#define LOADROW TRUE
//#if (defined(ROW_BUFFER) && ROW_BUFFER)
//#error
//#include "rowBuffer.c"
//#endif

#if (defined(SEG_TET) && SEG_TET)
 #include "_options/tetStuff.c"
#endif


#if (defined(BLUE_ADC) && BLUE_ADC)
 #include _ADC_HEADER_
#endif

#if (defined(PIEZO_HIT_DETECTION) && PIEZO_HIT_DETECTION)
//#if (defined(FB_QUESTION) && FB_QUESTION)
 #include _PIEZOHITDETECTOR_CFILE_
#endif

#define OSCCAL_VAL	0xff//0xDB//0xDC//0xE0//0//0xff//0x20//0xff //0x00
#include "delay_cyc.c"
#include "lcdDefines.h"

//Now included in lvds[161].c:
//#include "pll.c"





#if(defined(FB_SMILEY) && FB_SMILEY)
 #define FB_WIDTH 16
 #define FB_HEIGHT 16
#elif (defined(FB_QUESTION) && FB_QUESTION)
 #define FB_WIDTH 16
 #define FB_HEIGHT 16
#elif (defined(FB_TETRIS) && FB_TETRIS)
 #define FB_WIDTH	24
 #define FB_HEIGHT 24
#elif (defined(FB_HEXCOLOR) && FB_HEXCOLOR)
 #define FB_WIDTH 16
 #define FB_HEIGHT 16
#endif

#if (defined(FB_QUESTION) && FB_QUESTION)
 #if (defined(BUMP_SWITCH) && BUMP_SWITCH)
  //#error "OK1"
  #include "_hitSensors/bumpSwitch.c"
 #endif
#endif

#include _LCD_INTERFACE_CFILE_
/*
#if (defined(__AVR_AT90PWM161__))
 #include "_interfaces/lvds161.c"
#else
 #include "_interfaces/lvds.c"
#endif
*/


#if(defined(FB_SMILEY) && FB_SMILEY)
 #define fb_updater()	smileyUpdate()
#elif (defined(FB_QUESTION) && FB_QUESTION)
 #include "fb_question.h"
 #define fb_updater()	fbQuestion_update()
#elif (defined(FB_TETRIS) && FB_TETRIS)
//These are just rough estimates...
 #include "_options/tetStuff.c"
 #define fb_updater()	tetUpdate()
#elif (defined(FB_HEXCOLOR) && FB_HEXCOLOR)
 #include "_options/hexColor.c"
 #define fb_updater()	hexColor_update()
#endif

uint8_t isNewFrame(void);

#if ( (defined(FRAMEBUFFER_TESTING) && FRAMEBUFFER_TESTING) || \
		(defined(ROWBUFFER_TESTING) && ROWBUFFER_TESTING) )
 #include "_options/writeColor.c"
#endif




#if (defined(FB_QUESTION) && FB_QUESTION)
 #include "fb_question.c"
#endif

// a/o v59:
//PSEGS are the "pixel segments" addressable in RowSegmentBuffer
// e.g. one could load a single row-segment NUM_PSEGS long, and it would 
// reach from one edge of the screen to the other
// or any number <=NUM_SEGMENTS of row segments whose lengths all add up to
// <=NUM_PSEGS. (It's possible to extend beyond NUM_PSEGS, which would draw
// beyond the edge of the screen, but it may affect the screen's ability 
// to sync properly)
// This value isn't exactly adjustable, it's a characteristic of the
// ratio of the pixel-clock and the system-clock, along with the number of
// system clock cycles used to draw each pixel.
// This value was determined by experimentation with the very specific 
// configuration currently in use, though it could probably be calculated 
// from other #defines elsewhere.
// (and probably *should* be, at some point, especially with 
//  experimentations with PLLSYSCLK, LVDS_PRESCALER, SLOW_LVDS, etc...)
// For now, changing this doesn't really do anything except scale some 
// drawings horizontally (E.G. SEG_RACER)
//These notes were determined long ago when developing SEG_RACER:
// Wasn't sure where 357 came from... and now I'm wondering where the
// notes I wrote regarding it disappeared to...
// Anyhow, from line 72, if ROW_SEG_BUFFER && !LVDS_PRESCALER
// "ROW_SEG_BUFFER uses 20cyc/pixel" (doesn't match the math I've 
// elsewhere) 1024/20*7 = 358.4... so maybe that's something to do with it
// TODO: If it was 21cyc/pixel, wouldn't we have perfect alignment with
//       color-transitions and segment widths?
//#define NUM_PSEGS	(1024/3+16) //357

#if (LVDS_PRESCALER == 1)
 #warning "LVDS_PRESCALER == 1 is used extensively a/o v62 to indicate that	we're testing v62... this is hokey, for sure."
 //Let's see if this math can be figured out...
 //
 // Hah, right??? I completely forgot about SQUARE_SEGMENTS in all this
 // Assuming they're always square, for now.
 //
 // H_PIXELS (DE_ACTIVE_DOTS)		1024             pixels
 
 // LVDS_PRESCALER					(1,2,4,8...)     pll_cyc/lvds_bit
 //    PLLCYC_PER_LVDSBIT        LVDS_PRESCALER

 // LVDS_BITS_PER_PIXEL				7                lvds_bits/pixel
 // PLL_SYSCLK
 //    TRUE:  PLLCYC_PER_CPUCYC  4                pll_cyc/cpu_cyc
 //    FALSE: PLLCYC_PER_CPUCYC  8                pll_cyc/cpu_cyc
 // DRAWSEG_CPU_CYCS_PER_PSEG		20               cpu_cyc/pseg
 //
 // PIXELS_PER_PSEG = 1pseg * CPUCYCS_PER_PSEG   -> cpu_cycs
 //                         * PLLCYC_PER_CPUCYC  -> pll_cycs
 //                         / PLLCYC_PER_LVDSBIT -> lvds_bits
 //                         / LVDSBITS_PER_PIXEL -> pixels
 //   This is where dimensional-analysis always bites me in the ass...
 //   my units are now PIXELS_PER_PSEG = pix/pseg = pix
 //   I THINK (and this is an epiphany after years of recurring troubles
 //   here) it's possible to, instead, note that 1pseg came from *nowhere*
 //   so should be 1pseg/1pseg * <all the dimensional-analysis stuff>
 //   Then:
 //   (Note, in most cases PIXELS_PER_PSEG will be small, and here
 //   fractions are highly important...)
 #define CPUCYC_PER_PSEG	(DRAWSEG_CPU_CYCS_PER_PSEG)
 #if(defined(PLL_SYSCLK) && PLL_SYSCLK)
  #define PLLCYC_PER_CPUCYC	4
 #else
  #define PLLCYC_PER_CPUCYC	8
 #endif
 #define PLLCYC_PER_LVDSBIT	(LVDS_PRESCALER)
 #define LVDSBITS_PER_PIXEL	7

 #define PIXELS_PER_PSEG_F ( MAKEFLOAT( CPUCYC_PER_PSEG   ) \
		 							* MAKEFLOAT( PLLCYC_PER_CPUCYC  ) \
		 							/ MAKEFLOAT( PLLCYC_PER_LVDSBIT ) \
		 							/ MAKEFLOAT( LVDSBITS_PER_PIXEL ) )
 // NUM_PSEGS = H_PIXELS / PIXELS_PER_PSEG
 #define NUM_PSEGS_F (MAKEFLOAT(DE_ACTIVE_DOTS) / PIXELS_PER_PSEG_F)
 // And here, we're leaving NUM_PSEGS_F as a float so it can be used in 
 // macro-comparisons, if necessary...
 // (I don't think MAKELONG or any other such macro could handle this
 // casting... it may be redundant since most uses would be assignments in
 // integers, but if math is used, we don't want floating-point math to be
 // run on it in real-time calculations...)
 #define NUM_PSEGS ((uint16_t)(NUM_PSEGS_F))
 // As a hand-job test: Using old values (when 357 was determined)
 // DE_ACTIVE_DOTS = 1024
 // CPUCYCS_PER_PSEG = 20
 // PLLCYC_PER_CPUCYC = 8
 // PLLCYC_PER_LVDSBIT =8
 // LVDSBITS_PER_PIXEL =7
 // 1024 / (20 * 8 / 8 / 7) = 358.4
 // (The old NUM_PSEGS = 1024/3+16 = 357.333)
 // (As an aside, if CPUCYCS_PER_PSEG = 21: 1024/(21*8/8/7)=341.333 
 //    == exactly three pixels per pseg)
#else
 #warning "NUM_PSEGS is hardcoded, but should be calculated by many factors, such as LVDS_PRESCALER, and PLL_SYSCLK..."
 #define NUM_PSEGS (1024/3+16)
#endif

//Yes, I know #including C files is bad-practice...
// and yes, this has to be here... for now.
#if(defined(SEG_RACER) && SEG_RACER)
 #include "seg_racer.c"
#endif





// a/o v60, this note is OLD, and actually is the whole point behind
// ROW_SEG_BUFFER's current implementation.
// TODO: Between OSCCAL_VAL and FRAME_UPDATE_DELAY
//  it seems the speed (at least for the LTN) isn't so important after all
//  This could be used to my advantage...
//  Maybe bump OSCCAL_VAL all the way up, but use a slower "bit-rate"
//  Thus, pixels could be skinnier, thus higher-resolution.
//   Or more instructions could be used for the same resolution
//   allowing, e.g. sprites from program-memory, without necessitating a
//   large (or any?) frame-buffer...


//This used to be handled by TOOFAST_TEST, but now can be configured here
// 0xff sets the processor to run as fast as possible 
//  (~16MHz? ~128MHz bit-rate)
// 0x00 sets it as slow as possible (~4MHz, ~32MHz bit-rate)
//  good for testing whether odd colors
//  or bad syncing is due to bits being shifted, etc. 
//  Maybe due to slow "LVDS" conversion chips, or whatnot.
//   This seems to be the case right now... 0xff: black is appearing green
//   There should be *some* green in it, but not this much.
//   Likely due to different propagation delays between my under-driven
//    74LS86's (running red and green), from 3.3V!
//    and the happier and newer 74AHC XOR and OR currently driving the
//    clock and DVH/Blue signals
//  Highly dependent on the screen itself, some may not run at all with
//  such slow bitrates... heck, some may not run at all even at the fastest
// If SLOW_EVERYTHING_TEST is true, this value is overridden
// LTN last used 0x20
// a/o v59:
// 0xD8 is the lowest value that syncs reliably, a few glitches from time
// to time.
// The effect of lower values isn't *quite* what I was expecting, but
// definitely suggests that using ROW_SEG_BUFFER at this bit/refresh-rate
// is dang-near right at the minimum required for this display.
// This might be why the other (same model) display didn't work, but its
// flakeyness at 0xff appeared different than this one's at lower OSCCALs
// --Later note... actually retrying the "flakey" display with the same
//   configuration. Actually, yes, the problem looks dang-near identical to
//   the "reliable" display's being run at OSCCAL_VALs lower than 0xD8
//   Both displays are of the same manufacturer/model.
//    "reliable" has the chip: LXD91810 VS252AG
//    "flakey"   has the chip: LXD91810 VS242AC
//   The only bit I don't get is that I'm dang-near certain I had both
//   displays working at one point, and from the same code...
//   but this OSCCAL value is the maximum, and the only other way to speed
//   up the bit-rate changes the resolution; which I haven't done
//   for quite some time.
//   Also different about this test than when I originally determined it
//   "flakey": I've since switched the 74LS00 and LS32 out for LS86's
//   (These are wired such that one chip is responsible for the + side of
//    every LVDS signal, and one chip is responsible for the - side.
//    The best-practice, probably, would be to have the + and - sides of
//    an LVDS signal on the same chip. It's also plausible that my test was
//    so long ago that it was on the old board which had this configuration
//    but I doubt that, since that board was so hokily-implemented (with
//    chip-clips and grabbers all over the place) that I can't imagine it
//    was ever running again after all those months of disuse. Though...
//    if that *was* the case... it could also be that I'm using a different
//    ATtiny861... in which case, maybe the RC oscillator and/or PLL aren't
//    capable of running as fast... 
//    Not that I have a ton of these things lying around (three total, in
//    various projects already)
//    BUT the end-result may be that I just happen to have the one display
//    and ATtiny861 on the planet that will work together. That'd be lame.)
//#define OSCCAL_VAL	0xff//0xDB//0xDC//0xE0//0//0xff//0x20//0xff //0x00
//This has been moved to the top...




//a/o v60: These are old experiments...
// they *are useful* for testing new displays...
// but I can't recall off-hand how to use them instead of (or with?)
// rowSegBuffer...
// They're defined and better-described in lcdStuff.c...
//These three are mutually-exclusive (BLUE_VERT_BAR overrides DE_BLUE)
// (BLUE_DIAG_BAR overrides both...)
//#define BLUE_DIAG_BAR TRUE
//A single black bar, followed by a single blue bar... ~1/3 and 2/3's width
//#define BLUE_VERT_BAR TRUE
//#define DE_BLUE TRUE
// OLD NOTE:
// NONE SELECTED DOESN'T SYNC!!! DE not detected???



// Default is drawPix, from program memory...
// a/o v60: drawPix() has since become the de-facto function for drawing
// a row of pixels, regardless of the method.
// It's defined as necessary...



//This is where COLOR_BAR_SCROLL and PIXEL_SCROLL were, in part.






//#include "lvds.c"



#if(defined(LOADROW) && LOADROW)
static __inline__ \
void loadRow(uint16_t rowNum) \
	__attribute__((__always_inline__));
#endif

#if(defined(PARTIAL_REFRESH) && PARTIAL_REFRESH)
#define FULL_REFRESH	UINT16_MAX
uint16_t stopRefreshAtRow = FULL_REFRESH;
#endif

//The endOfFrameHandler() function might e.g. cause the timer-interrupt to
//be disabled until it's time to redraw a new frame
// Since we're working with TFT displays, there is inherent "memory" of the
// last-written frame, so we needn't refresh at the maximum rate possible
// For some displays (e.g. the LTN) the act of refreshing is visible, so
// unless the image changes (or the pixels have faded) it would be best
// *not* to refresh
static __inline__
void endOfFrameHandler(void)
	__attribute__((__always_inline__));

#if(!defined(PWM_TESTING) || !PWM_TESTING)
 //This must (Now) come *after* the display headers, etc...
 // especially for LCDSTUFF_USE_DOTS_TO_CYC_IN_DELAY_DOTS
 #warning "This implementation is a bit hokey... LCDSTUFF_USE_DOTS_TO_CYC_IN_DELAY_DOTS"
 #if(defined(LCDINTERFACE_BITBANGED_DOTCLOCK) &&\
		 LCDINTERFACE_BITBANGED_DOTCLOCK)
	#warning "LCDSTUFF_USE_DOTS_TO_CYC_IN_DELAY_DOTS is FALSE (better be a bit-banged dot-clock, otherwise there's lots of math to be done...)"
	#define LCDSTUFF_USE_DOTS_TO_CYC_IN_DELAY_DOTS	FALSE
 #else
	#warning "LCDSTUFF_USE_DOTS_TO_CYC_IN_DELAY_DOTS is TRUE (doesn't work with bitbanged dot-clocks!)"
	#define LCDSTUFF_USE_DOTS_TO_CYC_IN_DELAY_DOTS	TRUE
 #endif
 #include _LCDSTUFF_CFILE_
 //#include "../../../_commonCode/lcdStuff/0.50ncf/lcdStuff.c"

	//If REFRESH_ON_CHANGE != TRUE, then FB_REFRESH_ON_CHANGE_COUNT = TRUE
	// and goes unchanged...

	//updateFrame is decremented in main.c each time the frame is completed
	// it's reset to FB_REFRESH_ON_CHANGE in _options/frameBuffer.c
	// (via restartFrameUpdate())
 volatile uint8_t updateFrame = FB_REFRESH_ON_CHANGE_COUNT; //TRUE;
 volatile uint8_t frameCount = 0;


 void restartFrameUpdate(void)
 {
	updateFrame = FB_REFRESH_ON_CHANGE_COUNT;

	//The sony display seems to dislike not having a dot-clock...
	// This is an experiment...
  #if (defined(LCDINTERFACE_BITBANGED_DOTCLOCK_PWM) && \
		  LCDINTERFACE_BITBANGED_DOTCLOCK_PWM)
	lcdInterface_pwmDotClockInit(FALSE);
  #endif
 }

 //Nearly everything display-related happens in this interrupt
 // It even calls the functions that load (and calculate!) the data for the
 // next row.
 // So basically, the entire project is running via timer-interrupt.
 SIGNAL(HSYNC_TIMER_INTERRUPT_VECT) //TIMER0_COMPA_vect)
 {
	 //This was an *early* test to compare functionality between v91 and
	 //V66.51 (which was working)
	 // Didn't seem to have any effect.
	 // Wasn't until later it was discovered that GCC4.4 was being used for
	 // v66.51, and therein lied the problem, not here.
	 // a/o v92, this has been disabled, and no noticeable change has
	 // appeared on the BOE display.
	 // Plus, the heart now works.
	 // This does NOT make v92+ compatible with V66.51-64 (the last version)
	 // but more like v66.51-1, so is basically irrelevent, now.
	 // (And doesn't take into account any changes made since v91, anyhow).
//#define V6651COMPARE TRUE

#if (!defined(V6651COMPARE) || !V6651COMPARE)
#if(defined(_DMS_EXTERNALUPDATE_) && _DMS_EXTERNALUPDATE_)
	 dms_update();
#endif

		//Since HEART_TCNTER_UPDATES_AND_INIT is not true (could test here)
		// we need to do it manually...
		// (because the tcnter uses the hsyncTimer)
#if(!defined(HEART_TCNTER_UPDATES_AND_INIT) || \
		!HEART_TCNTER_UPDATES_AND_INIT)
 #if(defined(_HEART_TCNTER_) && _HEART_TCNTER_)
	 tcnter_overflowUpdate();
 #endif
#endif

	 if(!updateFrame)
		 return;

 //	static uint8_t frameCount = 0;

	//Could insert a delay of sorts for scopability...
	// (otherwise there's not much guarantee that register-assignments
	//  later will align with the LVDS frame... though ALIGN should help)
	// see scopeHsync.c

	//So here's the deal:
	// This handles a single row of pixel/timing data (Hblanks included)
	// lcd_update()             (from lcdStuff.c) 
	//  --> loadData()         (from lcdStuff.c)
	//        HSync
	//        H-Back-Porch
	//        --> drawPix()    (drawPix() may be here or elsewhere)
	//        (H-Front-Porch)
	//
	// drawPix is only called for drawn-rows (where DE is active, NOT for
	// Vblank rows)
	// H-Front-Porch is handled in the time between completion of this
	// interrupt and the next interrupt...

#endif // testing 91 vs 66.51-1
	 //a/o v66.51-64: lcdUpdate() was surrounded by heart set/cleared
	 // for 'scoping syncing... definitely revealed the problem. But easy
	 // enough to do that the code's not reimplemented here (set/clrpinPORT)
	int16_t rowNum = lcd_update();

	if(rowNum == LCD_FRAMECOMPLETE)
	{
		frameCount++;
		//lcd_update() returns TRUE when the frame is complete
		// which can be used for whatever purposes 
		// (e.g. FRAME_COUNT_TO_DELAY)
#if(!defined(V6651COMPARE) || !V6651COMPARE)
		endOfFrameHandler();
#endif
	}

#if(!defined(V6651COMPARE) || !V6651COMPARE)
#if(defined(PARTIAL_REFRESH) && PARTIAL_REFRESH)
	else if(rowNum >= (int16_t)stopRefreshAtRow)
	{
		lcd_init();
		frameCount++;
		endOfFrameHandler();
	}
#endif
#endif

 #if(defined(LOADROW) && LOADROW)
	//a/o v70: All the following notes are old, and should probably be
	//removed...
	//  Historically: lcd_update() handled calling loadRow
	//  That'd been removed, then readded, then removed
	//  as, somewhere inbetween it was moved here and forgotten
	//  This makes the most sense, as it's more project-specific
	//
	//Load the next row into the row(seg)buffer
	//data is enabled after T_VD+T_Vlow...
	//a/o v60: Rereading this is confusing...
	// Basically, there's no reason to call loadRow for display-lines
	// which don't display data (e.g. V-Sync)
#warning "This test could probably be replaced with rowNum..."

	if((hsyncCount >= T_VD+T_Vlow) && (hsyncCount < T_VD+T_Vlow+V_COUNT))
		loadRow(hsyncCount - (T_VD+T_Vlow));
 #endif
	// a/o v60: This is a funny old note... I can't recall a time when
	// loadRow was written, then removed, then added again. Weird:
	// Here is where loadRow used to be called
	// it has been moved to oldNotes.txt, but it would make more sense
	// to look at LCDdirectLVDS <25 or LCDdirect
	// loadRow used the time between DE inactive and the next Hsync
	// to load a row to a row-buffer (as opposed to a frame-buffer)
	// thus, DE could gain more pixels because there would be fewer
	// calculations to determine the memory location to write from
	// This effect has since been minimized
	// BUT loadRow might be necessary for program-memory-based images...


	//THIS IS A HACK
	//tcnter_update();

 }
#endif //!PWM_TESTING


#if(defined(SEG_HFM) && SEG_HFM)
	#include "seg_hfm.c"
#endif

#if(defined(SEG_QUESTION) && SEG_QUESTION)
	#include "seg_question.c"
#endif

#if(defined(SEG_SINE) && SEG_SINE)
	#include "seg_sine.c"
#endif

#if(defined(LOADROW) && LOADROW)
void loadRow(uint16_t rowNum)
{


#if(defined(ROW_SEG_BUFFER) && ROW_SEG_BUFFER)
 #if (defined(SEG_RACER) && SEG_RACER)
	racer_loadRow(rowNum);
 #elif (defined(SEG_HFM) && SEG_HFM)
 	segHFM_loadRow(rowNum);
 #elif (defined(SEG_QUESTION) && SEG_QUESTION)
 	segQuestion_loadRow(rowNum);
 #elif (defined(SEG_SINE) && SEG_SINE)
 	segSine_loadRow(rowNum);
 #elif (defined(SEG_LINE) && SEG_LINE)
	//Left here for an example...
	// Draws a diagonal line 
	//OLD NOTE:
	//syncing issues due to recursion overflowing the stack???
	// Apparently was
	   
	//Isn't BLAH = 1 necessary so we don't get a row with no data?
	// *looks* like it's working, but I dunno...
	#define BLAH 0

	segClear();
	newSeg(3,0x06, (6<<4) | 3);									//W
	newSeg((rowNum&0xff) | BLAH, 0x06, (4<<4) | 0);				//R
	newSeg(1,0x06, (6<<4) | 3);									//W
	newSeg((255-(rowNum&0xff)) | BLAH, 0x06, (4<<4) | 0 );	//R
	newSeg(3,0x06, (6<<4) | 3);									//W
	segTerminate();

 #elif (defined(SEG_TET) && SEG_TET)
	//This isn't particularly functional, anymore
	// it used to be an intermediate stage between rowBuffer and 
	//  rowSegBuffer... Left here for an example of how it could be done...
	//ACTUALLY, a/o v60ish it has been reinstated...
	// and, in fact, a/o v62, this may become a great early-test for new
	// displays, as the necessary resolution is low, which would allow for
	// higher refresh-rates (which some displays may need)
	// (There are certainly other ways to make things more suitable for new
	// displays... these'll be explored in later code-revisions, probably)

  //SEG_STRETCH is the number of PSEGS that each row-buffer pixel is
  //stretched-across. In this case (SEG_TET) (or any case where *only* the
  //rowbuffer is used to fill the screen), the value of SEG_STRETCH should
  //probably be varied dependent on NUM_PSEGS (which should be dependent on
  //LVDS_PRESCALER and a few other factors) in order to stretch the
  //row-buffer acreoss the entire screen. But I think most of these things
  //are so-far hard-coded...
  // Further, it's entirely possible (in a not-implemented-here case) to
  // have the row-buffer occupy only a certain portion of the screen and
  // modify the remaining psegs directly via other means... (I had this
  // once, when displaying Tetris, Life, and a few others on the same
  // screen, I think, right?)
  //Anyhow, a/o a while back, SEG_TET is the only case using SEG_STRETCH 
//  #if(!defined(SEG_STRETCH))
// And a/o v62 Trying to bump the frame-rate up so the ChiMei display will
// work, using LVDS_PRESCALER, this might help... again hardcoded where
// ideally the thing would calculate itself... Though it's also handy to
// have it hard-coded as it allows for testing the display's immunity (or
// lack thereof) for long/short DEs
#if(LVDS_PRESCALER == 1)
 #define SEG_STRETCH 1
#else
 #define SEG_STRETCH 5 //(((NUM_PSEGS-6)+RB_WIDTH-1)/RB_WIDTH)
#endif
	//3-5 = white + cyan
	//6 = letters alternating with above
	//7-9 = ditto, stretched

#define TET_VSTRETCH	16
	if(rowNum == 0)
	{
		//Probably not best to put this here, as we're still in the interrupt
		// extra-long calculations might cause syncing issues with displays
		// that require rows to be a constant time
		tetUpdate();
	}
	if(rowNum % TET_VSTRETCH == 0)
	{
		uint8_t i;
		for(i=0; i<RB_WIDTH; i++)
			rowBuffer[i] = fb_to_rb(_K);
		//	rowBuffer[i] = fb_to_rb((i+rowNum/TET_VSTRETCH)&0x3f);
		
		tet_drawRow(rowNum/TET_VSTRETCH, rowBuffer);
	}
	segClear();

	//Good for syncing to have white on both borders...
	newSeg(3, 0x06, (6<<4) | 3);
	
	uint16_t i;
	//TET_OVERLAY alternates rows between the TETRIS board and
	// an rgb "gradient" The only purpose is to test the idea of
	// using the high vertical-resolution to essentially increase the
	// number of colors available to a low-resolution image
	// sort of like "dithering" in the ol' days of 256 colors in Windows
	// but easy to implement in this case with very little overhead 
	// row-by-row
	// The effect, from a ways back, looks like the colors are blended
	// (like the Text and Game-board are translucent)
#define TET_OVERLAY TRUE

	//TET_GRADIENT takes that a step further and attempts to create a
	// gradient between each color in that rgb-gradient.
	// It's not as pretty as I'd hoped... though it makes sense...
	// first we're alternating between TET's color and the background color
	// then the background color is (roughly) alternating between a couple
	// colors, several rows between...
	// so if a color is stretched 16 rows vertically, that only leaves 8
	// rows for the color-gradient... might work better over more gradual
	// color-changes
#define TET_GRADIENT TRUE

#if (defined(TET_OVERLAY) && TET_OVERLAY)
	if(rowNum & 0x01)
	{
#endif
	for(i=0; i<RB_WIDTH; i++)
	{
		//i+1 because we don't want to overwrite the white border...
		rbpix_to_seg(rowBuffer[i], i+1, SEG_STRETCH);
	}

#if (defined(TET_OVERLAY) && TET_OVERLAY)
	}
	else
	{
		#define ROWS_PER 64//(V_COUNT/NUM_COLORS)

		uint8_t fbColor;

#if (defined(TET_GRADIENT) && TET_GRADIENT)
		static hfm_t hfmGradient;
		if(rowNum%ROWS_PER == 0)
			hfm_setup(&hfmGradient, 0, ROWS_PER/2-4);

		//It's worth it to experiment with changing the range of the 
		// hfmGradient, ('-4' above and '+4' below).
		// to try to avoid one or two stray spikes
		// (e.g. power = 1, maxPower = 15, there'll be one bright row
		//  and 14 dark, it sticks out like a sore-thumb)
		//  These values are experimental, and entirely dependent on the
		//  values used...

		hfm_setPower(&hfmGradient, (rowNum/2)%(ROWS_PER/2)+4);

		if(hfm_nextOutput(&hfmGradient))
			fbColor = rgbGradient(ROWS_PER-1 - rowNum/(ROWS_PER)+16-1);
		else
#endif	//TET_GRADIENT
			fbColor = rgbGradient(ROWS_PER-1 - rowNum/(ROWS_PER)+16);

		addSegfb(RB_WIDTH*SEG_STRETCH, (fbColor));
		//newSeg(SEG_STRETCH, fb_to_seg((rowNum*64/768)&0x3f));

	}
#endif	//TET_OVERLAY
	//white...
	newSeg(3, 0x06, (6<<4) | 3);
	segTerminate();
 #elif (defined(SEG_GRADIENT) && SEG_GRADIENT)
	static hfm_t hfmGradient;
#define ROWS_PER	(255)	//(V_COUNT/3) == 256

	if(rowNum%ROWS_PER == 0)
		hfm_setup(&hfmGradient, 0, ROWS_PER);

	uint8_t color;

	uint16_t power;

	if(rowNum < ROWS_PER/16)
		rowNum = 0;
	else
		rowNum -= ROWS_PER/16;

	//Since there are four shades, there will be three gradients...
	if(rowNum < ROWS_PER)
	{
		//The ROWS_PER/16 stuff is to help alleviate sharp color-spikes
		// which occur early-on... see the explanation in SEG_TET.
		color = 0;
		//hfm_setPower(&hfmGradient, 
		power = (rowNum); // + ROWS_PER/16);
	}
	else if(rowNum < ROWS_PER*2)
	{
		color = 1;
		//hfm_setPower(&hfmGradient, 
		power = rowNum-(ROWS_PER); // + ROWS_PER/16;
	}
	else
	{
		color = 2;
		//hfm_setPower(&hfmGradient, 
		power = rowNum-(ROWS_PER*2); // + ROWS_PER/16;
	}


	if(power < ROWS_PER/16)
		power = 0;

	hfm_setPower(&hfmGradient, (power <= 255) ? power : 255);

	if(hfm_nextOutput(&hfmGradient))
		color++;

	color |= color<<2 | color<<4;

	segClear();
	addSegfb(3, _W);
	addSegfb(NUM_PSEGS-6, color); 
	addSegfb(3, _W);
	segTerminate();
 #elif(defined(SEG_GRADIENT2) && SEG_GRADIENT2)
   #error "Heh, never did implement this..."
 #else
	#error "Gotta select a SEG_... option, or create your own"
 #endif //SEG_ selection
#elif (defined(ROW_BUFFER) && (ROW_BUFFER))

//This is a *really* simple test for ROW_BUFFER
// It just cycles through the colors in diagonal stripes...
// Technically each color should be 16 pixels high, but it appears to be
// coming through much taller. Certainly this is display-specific
// timing-related issues... (This display is known to repeat rows when not
// syncing correctly)
// First-guess is ROW_CALCULATION_CYCS isn't right, but higher values is
// causing syncing problems.
// TODO, I guess.
	uint16_t col;
	for(col=0; col<RB_WIDTH; col++)
		rowBuffer[col] = fb_to_rb((col+rowNum)&0x3f);
		//rowBuffer[col] = fb_to_rb((col+rowNum/16)&0x3f);

#endif //ROW_SEG_BUFFER
}
#endif //LOAD_ROW

#include "hsyncTimerStuff.c"


#if (!defined(EXTERNAL_DRAWPIX) || !EXTERNAL_DRAWPIX)
#if (defined(ROW_SEG_BUFFER) && ROW_SEG_BUFFER)
DP_INLINEABLE void drawPix(uint16_t rowNum)
{
	//Note that rowNum isn't really used here...
	// and it's only a uint8_t!
	rsb_drawPix(rowNum);
}

#elif (defined(BLUE_ADC) && BLUE_ADC)
//This is pretty much identical to most of the BLUE_TESTING in lcdStuff.c
// with a slight twist...
DP_INLINEABLE void drawPix(uint16_t rowNum)
{
      //uint16_t blueCyc = DOTS_TO_CYC(rowNum);
      //uint16_t notBlueCyc = DOTS_TO_CYC(DE_ACTIVE_DOTS)-blueCyc;
		adc_startConversion();
		while(adc_isBusy())
			asm("nop;");

      uint16_t blueDots = adc_getValue();
      uint16_t notBlueDots = DE_ACTIVE_DOTS - blueDots;

      DEonly_fromNada();
      //delay_cyc(notBlueCyc);
      //delay_Dots(notBlueDots);
      DE_DotDelay(notBlueDots);
      DEblue_fromDEonly();
      //delay_cyc(blueCyc);
      //delay_Dots(blueDots);
      DE_DotDelay(blueDots);
      Nada_fromDEblue();
}

#elif (defined(BLUE_AND_COLORS) && BLUE_AND_COLORS)
// This draws four colors on the screen, black, red, blue, and cyan
// in order to test the other colors (red and green)
DP_INLINEABLE void drawPix(uint16_t rowNum)
{
	//DE_ACTIVE_DELAYABLE attempts to take into account the overhead of
	//DE_DotDelay, setColors(), etc...
	//It's entirely arbitrary, and dependent on CPU speed (PLL_SYSCLK)
	// and many other factors...
#if(defined(PLL_SYSCLK) && PLL_SYSCLK)
 #define DE_ACTIVE_DELAYABLE	(DE_ACTIVE_DOTS-100)
		//Attempting to stretch the diagonal from corner-to-corner...
		uint16_t blueDots = (uint32_t)DE_ACTIVE_DELAYABLE * (uint32_t)rowNum
		  										/ V_COUNT;
#else
		//Because, I guess, the math is too long without PLL_SYSCLK
		// since it's not syncing
 #define DE_ACTIVE_DELAYABLE	(DE_ACTIVE_DOTS-200)
		uint16_t blueDots = rowNum;
#endif
		uint16_t notBlueDots = DE_ACTIVE_DELAYABLE - blueDots;

		uint16_t halfNotBlueDots = notBlueDots/2;

		uint16_t halfBlueDots = blueDots/2;


		//fullRed(), noRed(), fullGreen(), and noGreen()
		// function differently than the DEblue...() functions.
		// Since "blue" is shared with the timing-signals,
		// changing a blue-value requires keeping in mind the previous and
		// next states of the timing (usually DE-active in all cases, but not
		// always).
		// Changing of the "red" and "green" signals is easier, since
		// doing-so doesn't affect anything else.

		DEonly_fromNada();

		//Black
		DE_DotDelay(halfNotBlueDots);
		
		//Red
		fullRed();
		DE_DotDelay(halfNotBlueDots);

		//Blue
		noRed();
		DEblue_fromDEonly();
		
		DE_DotDelay(halfBlueDots);

		//Cyan
		fullGreen();
		
		DE_DotDelay(halfBlueDots);

		noGreen();
		Nada_fromDEblue();
}
#elif (defined(BLUE_ALLSHADES) && BLUE_ALLSHADES)
//This is an attempt at showing all available shades of blue
// Originally it was determined that 11 shades could be shown
// but there seems to be some trouble with using wrap-around values
// (e.g. when reset occurs before set in the count)
// It works fine when it's the *only* shade shown, but if shown with other
// shades on the screen, it causes syncing problems
// So, it's been determined that there are basically four discernable
// shades of blue which are usable with the PWM161
//
// The original idea: BLUE_ALLSHADES=TRUE, ALLSHADES_GRADIENT=FALSE
//  would fade the screen from black to bright blue
//  (it makes use of the wraparound-values)
// Then ALLSHADES_GRADIENT would display them all on the screen at once
// 
// It's since been somewhat hacked, to disable the unstable wrap-around
// values...
// Then ALLSHADES_AND_COLORS was added, to show all usable colors
// This's gotten a bit bloated, and would probably be better-implemented as
// several different BLUE_WHATEVERs
DP_INLINEABLE void drawPix(uint16_t rowNum)
{
#define BLUESHADES_MAX 11
		
#if(!defined(ALLSHADES_GRADIENT) || !ALLSHADES_GRADIENT)
 #define DOTSPERSHADE (DE_ACTIVE_DOTS) ///BLUESHADES_MAX)
		static uint8_t frameCount = 0;
		static uint8_t shadeNum = 0;

		if(rowNum == 0)
			frameCount++;

		if(frameCount == 60)
		{
			frameCount = 0;
			shadeNum++;
			if(shadeNum >= BLUESHADES_MAX)
				shadeNum = 0;
		}
#else
 #if(defined(ALLSHADES_AND_COLORS) && ALLSHADES_AND_COLORS)
		setRed4(4*rowNum/V_COUNT);
		setGreen4((16*rowNum/V_COUNT)%4);
		/*
		//K, R, G, Y
		if(rowNum < V_COUNT/4)
		{
			noRed();
			noGreen();
		}
		else if(rowNum < 2*V_COUNT/4)
		{
			fullRed();
			noGreen();
		}
		else if(rowNum < 3*V_COUNT/4)
		{
			fullGreen();
			noRed();
		}
		else
		{
			fullGreen();
			fullRed();
		}
		*/
 #endif
 //Add a little bit of extra delay for calculations...
 #define START_SHADE 0//10 //5 //10
 #define END_SHADE   10//0 //7 //10 //0

 #define SHADE_DIR_SIGN ((END_SHADE > START_SHADE) ? +1 : -1)
 #define NUM_SHADES  (((SHADE_DIR_SIGN > 0) ? (END_SHADE-START_SHADE) \
			 :(START_SHADE-END_SHADE))+1)

// Weird... with +0, syncing is way-off (such that the red/green doesn't
// come through *at all*
// and so-forth.
 #define DOTSPERSHADE (DE_ACTIVE_DOTS/(NUM_SHADES+15))
		int8_t shadeNum;

		DEonly_init();

		for(shadeNum=START_SHADE; shadeNum != (END_SHADE+SHADE_DIR_SIGN);
				shadeNum+=SHADE_DIR_SIGN)
		{

#endif
		uint8_t ocr2sa_val;
		uint8_t ocr2ra_val;

		//Black
//		DE_DotDelay(DOTSPERSHADE);

		//The commented-out cases are those that require wrapping
		// (e.g. "set" occurs *after* "reset")
		// They don't seem to work too well, when switching back and forth
/*		switch(shadeNum)
		{
			//Black	//these're from DEonly_init();
			case 0:
				ocr2sa_val = 1;
				ocr2ra_val = 4;
				break;
			case 1:
				//4:
				ocr2sa_val = 0;
				ocr2ra_val = 4;	
				break;
//			case 2:
//				//12:
//				ocr2sa_val = 6;
//				ocr2ra_val = 4;	
//				break;
//			case 3:
//				//28:
//				ocr2sa_val = 5;
//				ocr2ra_val = 4;	
//				break;
			case 2:
				//32:
				ocr2sa_val = 1;
				ocr2ra_val = 5;	
				break;
			case 3:
				//36:
				ocr2sa_val = 0;
				ocr2ra_val = 5;	
				break;
//			case 6:
//				//44:
//				ocr2sa_val = 6;
//				ocr2ra_val = 5;	
//				break;
			case 4:
				//48:
				ocr2sa_val = 1;
				ocr2ra_val = 6;	
				break;
			case 5:
				//52:
				ocr2sa_val = 0;
				ocr2ra_val = 6;	
				break;
//			case 9:
//				//56:
//				ocr2sa_val = 1;
//				ocr2ra_val = 0;	
//				break;
			default:
				//60:
				ocr2sa_val = 0;
				ocr2ra_val = 8;	//7 dun woik... all-low
				break;
		}

		//DEonly_fromNada();
	
		//setbit(PLOCK2, PCNF2);

		lockPSC2();
		OCR2SAL = ocr2sa_val;
		OCR2RAL = ocr2ra_val;
		unlockPSC2();
		//clrbit(PLOCK2, PCNF2);
*/
		uint8_t newShade = shadeNum; //rowNum*64/V_COUNT) % NUM_SHADES;

		if(newShade <= 3)			//0, 1, 2, 3
			newShade = newShade;
		else if(newShade <= 6)   //4->2, 5->1, 6->0
			newShade = (6-newShade);
		else
			newShade = 3;

		setBlue4(newShade);

		DE_DotDelay(DOTSPERSHADE);

#if(defined(ALLSHADES_GRADIENT) && ALLSHADES_GRADIENT)
		}
 #if(defined(ALLSHADES_AND_COLORS) && ALLSHADES_AND_COLORS)
		setRed4(0); //4*rowNum/V_COUNT);
		setGreen4(0); //(16*rowNum/V_COUNT)%4);
 #endif
#endif
		Nada_init(); //fromDEblue();
}

#elif (defined(PWM_TESTING) && PWM_TESTING)
	//No drawPix necessary...

#else
#if 0
#define drawPix nonRSB_drawPix
#else
//#include "_options/writeColor.c"
//#include "nonRSB_drawPix.c"
DP_INLINEABLE void drawPix(uint16_t rowNum)
{
	//a/o v70, it's in use again for ROW_BUFFER.
	//a/o v67, this is used by FRAMEBUFFER_TESTING...
	//Old:
	//This hasn't been used in quite some time... 
	// it may not work at all anymore.
	nonRSB_drawPix(rowNum);
}
#endif
#endif
#endif


uint8_t isNewFrame(void)
{
	uint8_t newFrame = FALSE;

	static uint8_t lastFrameCount = 0;
#define FRAME_COUNT_LIMIT 0x03
	uint8_t thisFrameCount = frameCount&FRAME_COUNT_LIMIT;

//		if(( (thisFrameCount==0) && (lastFrameCount==FRAME_COUNT_LIMIT) ))
	if(thisFrameCount != lastFrameCount)
		newFrame = TRUE;

	lastFrameCount = thisFrameCount;

	return newFrame;
}



#if(defined(FB_REFRESH_ON_CHANGE) && FB_REFRESH_ON_CHANGE)
//For FB_QUESTION, FB_SMILEY, etc. it might be nice to only refresh when 
// the actual image changes...
// This is revisited from _unusedIdeas/frameCountToDelay.c
void endOfFrameHandler(void)
{
	//The original plan was to stop the timer, but now that is used by
	//dmsTimer... so instead:
	if(updateFrame)
		updateFrame--; //= FALSE;

	//The sony display seems to dislike not having a dot-clock...
	// This is an experiment...
  #if (defined(LCDINTERFACE_BITBANGED_DOTCLOCK_PWM) && \
		  LCDINTERFACE_BITBANGED_DOTCLOCK_PWM)
	if(!updateFrame)
		lcdInterface_pwmDotClockInit(TRUE);
  #endif
}
#else
//See the declaration, above...
void endOfFrameHandler(void)
{
}
#endif

//#define TEST__FLASH TRUE

#if(defined(TEST__FLASH) && TEST__FLASH)
 #include "test__flash.c"
#endif



//v66.51-64:
// This was added for comparing the assembly-output of GCC4.4 vs 4.8
// Otherwise it's not-used
/*
void delayDotsFn(int32_t numDots)
{
	int32_t numCyc = DOTS_TO_CYC(numDots);

	delay_cyc(numCyc);
}
*/





int main(void)
{
// a/o FB_TETRIS, testing...
	//WTF... this ain't showin' up when FB_WIDTH=24...?!

#if(defined(FRAMEBUFFER_TESTING) && FRAMEBUFFER_TESTING)
	frameBufferInit();
	uint8_t i, j;
	for(i=0; i<FB_HEIGHT; i++)
		for(j=0; j<FB_WIDTH; j++)
			frameBuffer[i][j] = i+j;
#endif

#if(defined(SEG_RACER) && SEG_RACER)
	racer_init();
#endif

#if (defined(BLUE_ADC) && BLUE_ADC)
	adc_takeInput(6);
	adc_init();
	adc_select(6);
#endif


#if (defined(PIEZO_HIT_DETECTION) && PIEZO_HIT_DETECTION)
//#if (defined(FB_QUESTION) && FB_QUESTION)
	//This is identical to the BLUE_ADC case, above, but the adc is used
	//differently... and I'm trying to do some cleanup, which is more messy
	//here, because of it. Wee!

	//Actually, it runs adc_startConversion() at the end, so it's no longer
	//*identical*
	phd_init();
#endif

#if((defined(SEG_TET) && SEG_TET) || (defined(FB_TETRIS) && FB_TETRIS))
	tetInit(3);
#endif

#if(defined(_DMS_EXTERNALUPDATE_) && _DMS_EXTERNALUPDATE_)
	//These values could probably be calculated...
	// (as long as OSCCAL_VAL isn't changed)
	// But I'm going on the fact, it's refreshing about 8Hz
	// and there's roughly 768 rows (HSYNC interrupts)
	// (of course, there's vsync rows, too)
	// so... that's about .000125s per HSYNC interrupt
	// or 8000 interrupts per second
	// Which is roughly 1dms per interrupt...
	init_dmsExternalUpdate(1,1);
#endif



#if(!defined(PWM_TESTING) || !PWM_TESTING)
	init_hsyncTimer();
#endif

	//This starts pretty late... watch out for WDT
#if(!defined(V6651COMPARE) || !V6651COMPARE)
	init_heartBeat();

	setHeartRate(0);
#endif



#if(defined(TEST__FLASH) && TEST__FLASH)
	test__flash();
#endif






	lvds_timerInit();


#if (defined(FB_QUESTION) && FB_QUESTION)
 #if (defined(BUMP_SWITCH) && BUMP_SWITCH)
	hitSensor_init();
 #endif
#endif


	while(1)
	{
#if(defined(FB_QUESTION) && FB_QUESTION)
//THIS PROBABLY SHOULD BE REVISED...
//_hitSensors/bumpSwitch.c handles it differently...
//#if (!defined(BUMP_SWITCH) || !BUMP_SWITCH)
// I don't see why BUMP_SWITCH can't be true as well as
// heartPinInputPoll...
// But PIEZO_HIT_DETECTION is typically on the same pin as BUMP_SWITCH

#if(!defined(__HEART_REMOVED__) || !(__HEART_REMOVED__))
#if (defined(PIEZO_HIT_DETECTION) && PIEZO_HIT_DETECTION)
#if (defined(BUMP_SWITCH) && BUMP_SWITCH)
#error "BUMP_SWITCH and PIEZO_HIT_DETECTION can't *both* be enabled, currently"
#endif
		if(phd_update())				
#else
		if(!heartPinInputPoll())
#endif
			fbQuestion_hitDetected();
#endif
#endif

//#endif

//#if( (defined(FB_QUESTION) && FB_QUESTION) 
//	  ||	(defined(FB_SMILEY) && FB_SMILEY) )
#if( defined(FRAMEBUFFER_TESTING) && (FRAMEBUFFER_TESTING))
		frameBufferUpdate();
#endif

#if(!defined(V6651COMPARE) || !V6651COMPARE)
		heartUpdate();
#endif		
		//THIS IS A HACK
		//tcnter_update();
	}

}


#if(defined(ROW_SEG_BUFFER) && ROW_SEG_BUFFER)
 #include "rowSegBuffer.c"
#endif

/* mehPL:
 *    I would love to believe in a world where licensing shouldn't be
 *    necessary; where people would respect others' work and wishes, 
 *    and give credit where it's due. 
 *    A world where those who find people's work useful would at least 
 *    send positive vibes--if not an email.
 *    A world where we wouldn't have to think about the potential
 *    legal-loopholes that others may take advantage of.
 *
 *    Until that world exists:
 *
 *    This software and associated hardware design is free to use,
 *    modify, and even redistribute, etc. with only a few exceptions
 *    I've thought-up as-yet (this list may be appended-to, hopefully it
 *    doesn't have to be):
 * 
 *    1) Please do not change/remove this licensing info.
 *    2) Please do not change/remove others' credit/licensing/copyright 
 *         info, where noted. 
 *    3) If you find yourself profiting from my work, please send me a
 *         beer, a trinket, or cash is always handy as well.
 *         (Please be considerate. E.G. if you've reposted my work on a
 *          revenue-making (ad-based) website, please think of the
 *          years and years of hard work that went into this!)
 *    4) If you *intend* to profit from my work, you must get my
 *         permission, first. 
 *    5) No permission is given for my work to be used in Military, NSA,
 *         or other creepy-ass purposes. No exceptions. And if there's 
 *         any question in your mind as to whether your project qualifies
 *         under this category, you must get my explicit permission.
 *
 *    The open-sourced project this originated from is ~98% the work of
 *    the original author, except where otherwise noted.
 *    That includes the "commonCode" and makefiles.
 *    Thanks, of course, should be given to those who worked on the tools
 *    I've used: avr-dude, avr-gcc, gnu-make, vim, usb-tiny, and 
 *    I'm certain many others. 
 *    And, as well, to the countless coders who've taken time to post
 *    solutions to issues I couldn't solve, all over the internets.
 *
 *
 *    I'd love to hear of how this is being used, suggestions for
 *    improvements, etc!
 *         
 *    The creator of the original code and original hardware can be
 *    contacted at:
 *
 *        EricWazHung At Gmail Dotcom
 *
 *    This code's origin (and latest versions) can be found at:
 *
 *        https://code.google.com/u/ericwazhung/
 *
 *    The site associated with the original open-sourced project is at:
 *
 *        https://sites.google.com/site/geekattempts/
 *
 *    If any of that ever changes, I will be sure to note it here, 
 *    and add a link at the pages above.
 *
 * This license added to the original file located at:
 * /Users/meh/_avrProjects/LCDdirectLVDS/93-checkingProcessAgain/main.c
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 */
