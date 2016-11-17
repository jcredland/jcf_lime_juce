/** @file
  FontAwesome provides a range of modern 2D symbols.  See 

	http://fortawesome.github.io/

  This file makes it easy to use in JUCE software.   It's *great* for instantly 
  useful icons.

  Font Awesome is licensed under the OFL v1.1.  Full details are available online.
  
  The OFL allows the licensed fonts to be used, studied, modified and
  redistributed freely as long as they are not sold by themselves. The
  fonts, including any derivative works, can be bundled, embedded,
  redistributed and/or sold with any software provided that any reserved
  names are not used by derivative works. The fonts and derivatives,
  however, cannot be released under any other type of license. The
  requirement for fonts to remain under this license does not apply
  to any document created using the fonts or their derivatives.

*/

#ifndef FONTAWESOME_H_INCLUDED
#define FONTAWESOME_H_INCLUDED

namespace FontAwesomeData {
	extern const char*  FontAwesome_otf;
	const int           FontAwesome_otfSize = 106260;
};


/**
	See the static method drawIcon for an example of how to use the typeface.
*/
class FontAwesomeIcons {
public:
	/** Returns a Typeface::Ptr for FontAwesome.  */
	static Typeface::Ptr getTypeface()
	{
		static Typeface::Ptr typeface = Typeface::createSystemTypefaceFor(FontAwesomeData::FontAwesome_otf, FontAwesomeData::FontAwesome_otfSize);
		return typeface;
	}
	/** 
	 * Draws a font awesome icon, in the current colour, on some area of the screen 
	 * FontAwesomeIcons::draw(g, FontAwesomeIcons::faAnchor, someDrawArea); 
	 */
	static void drawIcon(Graphics & g, int iconCode, const Rectangle<float> & area)
	{
		g.saveState();
		g.setFont(Font(getTypeface()).withHeight(area.getHeight()));
		g.drawText(String::charToString(iconCode), area, Justification::centred, false); 
		g.restoreState();
	}

  static void drawIconRotated(Graphics & g, int iconCode, const Rectangle<float> & area, float radians)
  {
    g.saveState();
    
    GlyphArrangement icon;
    icon.addFittedText(Font(getTypeface()).withHeight(area.getHeight()), String::charToString(iconCode),
                       area.getX(), area.getY(), area.getWidth(), area.getHeight(),
                       Justification::centred, 1);
    
    icon.draw(g, AffineTransform::rotation(radians, area.getCentreX(), area.getCentreY()));
    g.restoreState();
  }
  
  static DrawableText * createDrawable(int iconCode, float height = 20.0f, Colour fgColour = Colours::lightgrey)
	{
		auto d = new DrawableText();
		d->setColour(fgColour);
		d->setText(String::charToString(iconCode));
		d->setFont(Font(getTypeface()).withHeight(height), true);
		d->setJustification(Justification::centred);
		d->setBoundingBox(RelativeParallelogram(Rectangle<float>(0.0f, 0.0f, 20.0f, 20.0f)));
		return d;
	}

	static const int fa500px = 0xf26e;
	static const int faAdjust = 0xf042;
	static const int faAdn = 0xf170;
	static const int faAlignCenter = 0xf037;
	static const int faAlignJustify = 0xf039;
	static const int faAlignLeft = 0xf036;
	static const int faAlignRight = 0xf038;
	static const int faAmazon = 0xf270;
	static const int faAmbulance = 0xf0f9;
	static const int faAnchor = 0xf13d;
	static const int faAndroid = 0xf17b;
	static const int faAngellist = 0xf209;
	static const int faAngleDoubleDown = 0xf103;
	static const int faAngleDoubleLeft = 0xf100;
	static const int faAngleDoubleRight = 0xf101;
	static const int faAngleDoubleUp = 0xf102;
	static const int faAngleDown = 0xf107;
	static const int faAngleLeft = 0xf104;
	static const int faAngleRight = 0xf105;
	static const int faAngleUp = 0xf106;
	static const int faApple = 0xf179;
	static const int faArchive = 0xf187;
	static const int faAreaChart = 0xf1fe;
	static const int faArrowCircleDown = 0xf0ab;
	static const int faArrowCircleLeft = 0xf0a8;
	static const int faArrowCircleODown = 0xf01a;
	static const int faArrowCircleOLeft = 0xf190;
	static const int faArrowCircleORight = 0xf18e;
	static const int faArrowCircleOUp = 0xf01b;
	static const int faArrowCircleRight = 0xf0a9;
	static const int faArrowCircleUp = 0xf0aa;
	static const int faArrowDown = 0xf063;
	static const int faArrowLeft = 0xf060;
	static const int faArrowRight = 0xf061;
	static const int faArrowUp = 0xf062;
	static const int faArrows = 0xf047;
	static const int faArrowsAlt = 0xf0b2;
	static const int faArrowsH = 0xf07e;
	static const int faArrowsV = 0xf07d;
	static const int faAsterisk = 0xf069;
	static const int faAt = 0xf1fa;
	static const int faAutomobile = 0xf1b9;
	static const int faBackward = 0xf04a;
	static const int faBalanceScale = 0xf24e;
	static const int faBan = 0xf05e;
	static const int faBank = 0xf19c;
	static const int faBarChart = 0xf080;
	static const int faBarChartO = 0xf080;
	static const int faBarcode = 0xf02a;
	static const int faBars = 0xf0c9;
	static const int faBattery0 = 0xf244;
	static const int faBattery1 = 0xf243;
	static const int faBattery2 = 0xf242;
	static const int faBattery3 = 0xf241;
	static const int faBattery4 = 0xf240;
	static const int faBatteryEmpty = 0xf244;
	static const int faBatteryFull = 0xf240;
	static const int faBatteryHalf = 0xf242;
	static const int faBatteryQuarter = 0xf243;
	static const int faBatteryThreeQuarters = 0xf241;
	static const int faBed = 0xf236;
	static const int faBeer = 0xf0fc;
	static const int faBehance = 0xf1b4;
	static const int faBehanceSquare = 0xf1b5;
	static const int faBell = 0xf0f3;
	static const int faBellO = 0xf0a2;
	static const int faBellSlash = 0xf1f6;
	static const int faBellSlashO = 0xf1f7;
	static const int faBicycle = 0xf206;
	static const int faBinoculars = 0xf1e5;
	static const int faBirthdayCake = 0xf1fd;
	static const int faBitbucket = 0xf171;
	static const int faBitbucketSquare = 0xf172;
	static const int faBitcoin = 0xf15a;
	static const int faBlackTie = 0xf27e;
	static const int faBluetooth = 0xf293;
	static const int faBluetoothB = 0xf294;
	static const int faBold = 0xf032;
	static const int faBolt = 0xf0e7;
	static const int faBomb = 0xf1e2;
	static const int faBook = 0xf02d;
	static const int faBookmark = 0xf02e;
	static const int faBookmarkO = 0xf097;
	static const int faBriefcase = 0xf0b1;
	static const int faBtc = 0xf15a;
	static const int faBug = 0xf188;
	static const int faBuilding = 0xf1ad;
	static const int faBuildingO = 0xf0f7;
	static const int faBullhorn = 0xf0a1;
	static const int faBullseye = 0xf140;
	static const int faBus = 0xf207;
	static const int faBuysellads = 0xf20d;
	static const int faCab = 0xf1ba;
	static const int faCalculator = 0xf1ec;
	static const int faCalendar = 0xf073;
	static const int faCalendarCheckO = 0xf274;
	static const int faCalendarMinusO = 0xf272;
	static const int faCalendarO = 0xf133;
	static const int faCalendarPlusO = 0xf271;
	static const int faCalendarTimesO = 0xf273;
	static const int faCamera = 0xf030;
	static const int faCameraRetro = 0xf083;
	static const int faCar = 0xf1b9;
	static const int faCaretDown = 0xf0d7;
	static const int faCaretLeft = 0xf0d9;
	static const int faCaretRight = 0xf0da;
	static const int faCaretSquareODown = 0xf150;
	static const int faCaretSquareOLeft = 0xf191;
	static const int faCaretSquareORight = 0xf152;
	static const int faCaretSquareOUp = 0xf151;
	static const int faCaretUp = 0xf0d8;
	static const int faCartArrowDown = 0xf218;
	static const int faCartPlus = 0xf217;
	static const int faCc = 0xf20a;
	static const int faCcAmex = 0xf1f3;
	static const int faCcDinersClub = 0xf24c;
	static const int faCcDiscover = 0xf1f2;
	static const int faCcJcb = 0xf24b;
	static const int faCcMastercard = 0xf1f1;
	static const int faCcPaypal = 0xf1f4;
	static const int faCcStripe = 0xf1f5;
	static const int faCcVisa = 0xf1f0;
	static const int faCertificate = 0xf0a3;
	static const int faChain = 0xf0c1;
	static const int faChainBroken = 0xf127;
	static const int faCheck = 0xf00c;
	static const int faCheckCircle = 0xf058;
	static const int faCheckCircleO = 0xf05d;
	static const int faCheckSquare = 0xf14a;
	static const int faCheckSquareO = 0xf046;
	static const int faChevronCircleDown = 0xf13a;
	static const int faChevronCircleLeft = 0xf137;
	static const int faChevronCircleRight = 0xf138;
	static const int faChevronCircleUp = 0xf139;
	static const int faChevronDown = 0xf078;
	static const int faChevronLeft = 0xf053;
	static const int faChevronRight = 0xf054;
	static const int faChevronUp = 0xf077;
	static const int faChild = 0xf1ae;
	static const int faChrome = 0xf268;
	static const int faCircle = 0xf111;
	static const int faCircleO = 0xf10c;
	static const int faCircleONotch = 0xf1ce;
	static const int faCircleThin = 0xf1db;
	static const int faClipboard = 0xf0ea;
	static const int faClockO = 0xf017;
	static const int faClone = 0xf24d;
	static const int faClose = 0xf00d;
	static const int faCloud = 0xf0c2;
	static const int faCloudDownload = 0xf0ed;
	static const int faCloudUpload = 0xf0ee;
	static const int faCny = 0xf157;
	static const int faCode = 0xf121;
	static const int faCodeFork = 0xf126;
	static const int faCodepen = 0xf1cb;
	static const int faCodiepie = 0xf284;
	static const int faCoffee = 0xf0f4;
	static const int faCog = 0xf013;
	static const int faCogs = 0xf085;
	static const int faColumns = 0xf0db;
	static const int faComment = 0xf075;
	static const int faCommentO = 0xf0e5;
	static const int faCommenting = 0xf27a;
	static const int faCommentingO = 0xf27b;
	static const int faComments = 0xf086;
	static const int faCommentsO = 0xf0e6;
	static const int faCompass = 0xf14e;
	static const int faCompress = 0xf066;
	static const int faConnectdevelop = 0xf20e;
	static const int faContao = 0xf26d;
	static const int faCopy = 0xf0c5;
	static const int faCopyright = 0xf1f9;
	static const int faCreativeCommons = 0xf25e;
	static const int faCreditCard = 0xf09d;
	static const int faCreditCardAlt = 0xf283;
	static const int faCrop = 0xf125;
	static const int faCrosshairs = 0xf05b;
	static const int faCss3 = 0xf13c;
	static const int faCube = 0xf1b2;
	static const int faCubes = 0xf1b3;
	static const int faCut = 0xf0c4;
	static const int faCutlery = 0xf0f5;
	static const int faDashboard = 0xf0e4;
	static const int faDashcube = 0xf210;
	static const int faDatabase = 0xf1c0;
	static const int faDedent = 0xf03b;
	static const int faDelicious = 0xf1a5;
	static const int faDesktop = 0xf108;
	static const int faDeviantart = 0xf1bd;
	static const int faDiamond = 0xf219;
	static const int faDigg = 0xf1a6;
	static const int faDollar = 0xf155;
	static const int faDotCircleO = 0xf192;
	static const int faDownload = 0xf019;
	static const int faDribbble = 0xf17d;
	static const int faDropbox = 0xf16b;
	static const int faDrupal = 0xf1a9;
	static const int faEdge = 0xf282;
	static const int faEdit = 0xf044;
	static const int faEject = 0xf052;
	static const int faEllipsisH = 0xf141;
	static const int faEllipsisV = 0xf142;
	static const int faEmpire = 0xf1d1;
	static const int faEnvelope = 0xf0e0;
	static const int faEnvelopeO = 0xf003;
	static const int faEnvelopeSquare = 0xf199;
	static const int faEraser = 0xf12d;
	static const int faEur = 0xf153;
	static const int faEuro = 0xf153;
	static const int faExchange = 0xf0ec;
	static const int faExclamation = 0xf12a;
	static const int faExclamationCircle = 0xf06a;
	static const int faExclamationTriangle = 0xf071;
	static const int faExpand = 0xf065;
	static const int faExpeditedssl = 0xf23e;
	static const int faExternalLink = 0xf08e;
	static const int faExternalLinkSquare = 0xf14c;
	static const int faEye = 0xf06e;
	static const int faEyeSlash = 0xf070;
	static const int faEyedropper = 0xf1fb;
	static const int faFacebook = 0xf09a;
	static const int faFacebookF = 0xf09a;
	static const int faFacebookOfficial = 0xf230;
	static const int faFacebookSquare = 0xf082;
	static const int faFastBackward = 0xf049;
	static const int faFastForward = 0xf050;
	static const int faFax = 0xf1ac;
	static const int faFeed = 0xf09e;
	static const int faFemale = 0xf182;
	static const int faFighterJet = 0xf0fb;
	static const int faFile = 0xf15b;
	static const int faFileArchiveO = 0xf1c6;
	static const int faFileAudioO = 0xf1c7;
	static const int faFileCodeO = 0xf1c9;
	static const int faFileExcelO = 0xf1c3;
	static const int faFileImageO = 0xf1c5;
	static const int faFileMovieO = 0xf1c8;
	static const int faFileO = 0xf016;
	static const int faFilePdfO = 0xf1c1;
	static const int faFilePhotoO = 0xf1c5;
	static const int faFilePictureO = 0xf1c5;
	static const int faFilePowerpointO = 0xf1c4;
	static const int faFileSoundO = 0xf1c7;
	static const int faFileText = 0xf15c;
	static const int faFileTextO = 0xf0f6;
	static const int faFileVideoO = 0xf1c8;
	static const int faFileWordO = 0xf1c2;
	static const int faFileZipO = 0xf1c6;
	static const int faFilesO = 0xf0c5;
	static const int faFilm = 0xf008;
	static const int faFilter = 0xf0b0;
	static const int faFire = 0xf06d;
	static const int faFireExtinguisher = 0xf134;
	static const int faFirefox = 0xf269;
	static const int faFlag = 0xf024;
	static const int faFlagCheckered = 0xf11e;
	static const int faFlagO = 0xf11d;
	static const int faFlash = 0xf0e7;
	static const int faFlask = 0xf0c3;
	static const int faFlickr = 0xf16e;
	static const int faFloppyO = 0xf0c7;
	static const int faFolder = 0xf07b;
	static const int faFolderO = 0xf114;
	static const int faFolderOpen = 0xf07c;
	static const int faFolderOpenO = 0xf115;
	static const int faFont = 0xf031;
	static const int faFonticons = 0xf280;
	static const int faFortAwesome = 0xf286;
	static const int faForumbee = 0xf211;
	static const int faForward = 0xf04e;
	static const int faFoursquare = 0xf180;
	static const int faFrownO = 0xf119;
	static const int faFutbolO = 0xf1e3;
	static const int faGamepad = 0xf11b;
	static const int faGavel = 0xf0e3;
	static const int faGbp = 0xf154;
	static const int faGe = 0xf1d1;
	static const int faGear = 0xf013;
	static const int faGears = 0xf085;
	static const int faGenderless = 0xf22d;
	static const int faGetPocket = 0xf265;
	static const int faGg = 0xf260;
	static const int faGgCircle = 0xf261;
	static const int faGift = 0xf06b;
	static const int faGit = 0xf1d3;
	static const int faGitSquare = 0xf1d2;
	static const int faGithub = 0xf09b;
	static const int faGithubAlt = 0xf113;
	static const int faGithubSquare = 0xf092;
	static const int faGittip = 0xf184;
	static const int faGlass = 0xf000;
	static const int faGlobe = 0xf0ac;
	static const int faGoogle = 0xf1a0;
	static const int faGooglePlus = 0xf0d5;
	static const int faGooglePlusSquare = 0xf0d4;
	static const int faGoogleWallet = 0xf1ee;
	static const int faGraduationCap = 0xf19d;
	static const int faGratipay = 0xf184;
	static const int faGroup = 0xf0c0;
	static const int faHSquare = 0xf0fd;
	static const int faHackerNews = 0xf1d4;
	static const int faHandGrabO = 0xf255;
	static const int faHandLizardO = 0xf258;
	static const int faHandODown = 0xf0a7;
	static const int faHandOLeft = 0xf0a5;
	static const int faHandORight = 0xf0a4;
	static const int faHandOUp = 0xf0a6;
	static const int faHandPaperO = 0xf256;
	static const int faHandPeaceO = 0xf25b;
	static const int faHandPointerO = 0xf25a;
	static const int faHandRockO = 0xf255;
	static const int faHandScissorsO = 0xf257;
	static const int faHandSpockO = 0xf259;
	static const int faHandStopO = 0xf256;
	static const int faHashtag = 0xf292;
	static const int faHddO = 0xf0a0;
	static const int faHeader = 0xf1dc;
	static const int faHeadphones = 0xf025;
	static const int faHeart = 0xf004;
	static const int faHeartO = 0xf08a;
	static const int faHeartbeat = 0xf21e;
	static const int faHistory = 0xf1da;
	static const int faHome = 0xf015;
	static const int faHospitalO = 0xf0f8;
	static const int faHotel = 0xf236;
	static const int faHourglass = 0xf254;
	static const int faHourglass1 = 0xf251;
	static const int faHourglass2 = 0xf252;
	static const int faHourglass3 = 0xf253;
	static const int faHourglassEnd = 0xf253;
	static const int faHourglassHalf = 0xf252;
	static const int faHourglassO = 0xf250;
	static const int faHourglassStart = 0xf251;
	static const int faHouzz = 0xf27c;
	static const int faHtml5 = 0xf13b;
	static const int faICursor = 0xf246;
	static const int faIls = 0xf20b;
	static const int faImage = 0xf03e;
	static const int faInbox = 0xf01c;
	static const int faIndent = 0xf03c;
	static const int faIndustry = 0xf275;
	static const int faInfo = 0xf129;
	static const int faInfoCircle = 0xf05a;
	static const int faInr = 0xf156;
	static const int faInstagram = 0xf16d;
	static const int faInstitution = 0xf19c;
	static const int faInternetExplorer = 0xf26b;
	static const int faIntersex = 0xf224;
	static const int faIoxhost = 0xf208;
	static const int faItalic = 0xf033;
	static const int faJoomla = 0xf1aa;
	static const int faJpy = 0xf157;
	static const int faJsfiddle = 0xf1cc;
	static const int faKey = 0xf084;
	static const int faKeyboardO = 0xf11c;
	static const int faKrw = 0xf159;
	static const int faLanguage = 0xf1ab;
	static const int faLaptop = 0xf109;
	static const int faLastfm = 0xf202;
	static const int faLastfmSquare = 0xf203;
	static const int faLeaf = 0xf06c;
	static const int faLeanpub = 0xf212;
	static const int faLegal = 0xf0e3;
	static const int faLemonO = 0xf094;
	static const int faLevelDown = 0xf149;
	static const int faLevelUp = 0xf148;
	static const int faLifeBouy = 0xf1cd;
	static const int faLifeBuoy = 0xf1cd;
	static const int faLifeRing = 0xf1cd;
	static const int faLifeSaver = 0xf1cd;
	static const int faLightbulbO = 0xf0eb;
	static const int faLineChart = 0xf201;
	static const int faLink = 0xf0c1;
	static const int faLinkedin = 0xf0e1;
	static const int faLinkedinSquare = 0xf08c;
	static const int faLinux = 0xf17c;
	static const int faList = 0xf03a;
	static const int faListAlt = 0xf022;
	static const int faListOl = 0xf0cb;
	static const int faListUl = 0xf0ca;
	static const int faLocationArrow = 0xf124;
	static const int faLock = 0xf023;
	static const int faLongArrowDown = 0xf175;
	static const int faLongArrowLeft = 0xf177;
	static const int faLongArrowRight = 0xf178;
	static const int faLongArrowUp = 0xf176;
	static const int faMagic = 0xf0d0;
	static const int faMagnet = 0xf076;
	static const int faMailForward = 0xf064;
	static const int faMailReply = 0xf112;
	static const int faMailReplyAll = 0xf122;
	static const int faMale = 0xf183;
	static const int faMap = 0xf279;
	static const int faMapMarker = 0xf041;
	static const int faMapO = 0xf278;
	static const int faMapPin = 0xf276;
	static const int faMapSigns = 0xf277;
	static const int faMars = 0xf222;
	static const int faMarsDouble = 0xf227;
	static const int faMarsStroke = 0xf229;
	static const int faMarsStrokeH = 0xf22b;
	static const int faMarsStrokeV = 0xf22a;
	static const int faMaxcdn = 0xf136;
	static const int faMeanpath = 0xf20c;
	static const int faMedium = 0xf23a;
	static const int faMedkit = 0xf0fa;
	static const int faMehO = 0xf11a;
	static const int faMercury = 0xf223;
	static const int faMicrophone = 0xf130;
	static const int faMicrophoneSlash = 0xf131;
	static const int faMinus = 0xf068;
	static const int faMinusCircle = 0xf056;
	static const int faMinusSquare = 0xf146;
	static const int faMinusSquareO = 0xf147;
	static const int faMixcloud = 0xf289;
	static const int faMobile = 0xf10b;
	static const int faMobilePhone = 0xf10b;
	static const int faModx = 0xf285;
	static const int faMoney = 0xf0d6;
	static const int faMoonO = 0xf186;
	static const int faMortarBoard = 0xf19d;
	static const int faMotorcycle = 0xf21c;
	static const int faMousePointer = 0xf245;
	static const int faMusic = 0xf001;
	static const int faNavicon = 0xf0c9;
	static const int faNeuter = 0xf22c;
	static const int faNewspaperO = 0xf1ea;
	static const int faObjectGroup = 0xf247;
	static const int faObjectUngroup = 0xf248;
	static const int faOdnoklassniki = 0xf263;
	static const int faOdnoklassnikiSquare = 0xf264;
	static const int faOpencart = 0xf23d;
	static const int faOpenid = 0xf19b;
	static const int faOpera = 0xf26a;
	static const int faOptinMonster = 0xf23c;
	static const int faOutdent = 0xf03b;
	static const int faPagelines = 0xf18c;
	static const int faPaintBrush = 0xf1fc;
	static const int faPaperPlane = 0xf1d8;
	static const int faPaperPlaneO = 0xf1d9;
	static const int faPaperclip = 0xf0c6;
	static const int faParagraph = 0xf1dd;
	static const int faPaste = 0xf0ea;
	static const int faPause = 0xf04c;
	static const int faPauseCircle = 0xf28b;
	static const int faPauseCircleO = 0xf28c;
	static const int faPaw = 0xf1b0;
	static const int faPaypal = 0xf1ed;
	static const int faPencil = 0xf040;
	static const int faPencilSquare = 0xf14b;
	static const int faPencilSquareO = 0xf044;
	static const int faPercent = 0xf295;
	static const int faPhone = 0xf095;
	static const int faPhoneSquare = 0xf098;
	static const int faPhoto = 0xf03e;
	static const int faPictureO = 0xf03e;
	static const int faPieChart = 0xf200;
	static const int faPiedPiper = 0xf1a7;
	static const int faPiedPiperAlt = 0xf1a8;
	static const int faPinterest = 0xf0d2;
	static const int faPinterestP = 0xf231;
	static const int faPinterestSquare = 0xf0d3;
	static const int faPlane = 0xf072;
	static const int faPlay = 0xf04b;
	static const int faPlayCircle = 0xf144;
	static const int faPlayCircleO = 0xf01d;
	static const int faPlug = 0xf1e6;
	static const int faPlus = 0xf067;
	static const int faPlusCircle = 0xf055;
	static const int faPlusSquare = 0xf0fe;
	static const int faPlusSquareO = 0xf196;
	static const int faPowerOff = 0xf011;
	static const int faPrint = 0xf02f;
	static const int faProductHunt = 0xf288;
	static const int faPuzzlePiece = 0xf12e;
	static const int faQq = 0xf1d6;
	static const int faQrcode = 0xf029;
	static const int faQuestion = 0xf128;
	static const int faQuestionCircle = 0xf059;
	static const int faQuoteLeft = 0xf10d;
	static const int faQuoteRight = 0xf10e;
	static const int faRa = 0xf1d0;
	static const int faRandom = 0xf074;
	static const int faRebel = 0xf1d0;
	static const int faRecycle = 0xf1b8;
	static const int faReddit = 0xf1a1;
	static const int faRedditAlien = 0xf281;
	static const int faRedditSquare = 0xf1a2;
	static const int faRefresh = 0xf021;
	static const int faRegistered = 0xf25d;
	static const int faRemove = 0xf00d;
	static const int faRenren = 0xf18b;
	static const int faReorder = 0xf0c9;
	static const int faRepeat = 0xf01e;
	static const int faReply = 0xf112;
	static const int faReplyAll = 0xf122;
	static const int faRetweet = 0xf079;
	static const int faRmb = 0xf157;
	static const int faRoad = 0xf018;
	static const int faRocket = 0xf135;
	static const int faRotateLeft = 0xf0e2;
	static const int faRotateRight = 0xf01e;
	static const int faRouble = 0xf158;
	static const int faRss = 0xf09e;
	static const int faRssSquare = 0xf143;
	static const int faRub = 0xf158;
	static const int faRuble = 0xf158;
	static const int faRupee = 0xf156;
	static const int faSafari = 0xf267;
	static const int faSave = 0xf0c7;
	static const int faScissors = 0xf0c4;
	static const int faScribd = 0xf28a;
	static const int faSearch = 0xf002;
	static const int faSearchMinus = 0xf010;
	static const int faSearchPlus = 0xf00e;
	static const int faSellsy = 0xf213;
	static const int faSend = 0xf1d8;
	static const int faSendO = 0xf1d9;
	static const int faServer = 0xf233;
	static const int faShare = 0xf064;
	static const int faShareAlt = 0xf1e0;
	static const int faShareAltSquare = 0xf1e1;
	static const int faShareSquare = 0xf14d;
	static const int faShareSquareO = 0xf045;
	static const int faShekel = 0xf20b;
	static const int faSheqel = 0xf20b;
	static const int faShield = 0xf132;
	static const int faShip = 0xf21a;
	static const int faShirtsinbulk = 0xf214;
	static const int faShoppingBag = 0xf290;
	static const int faShoppingBasket = 0xf291;
	static const int faShoppingCart = 0xf07a;
	static const int faSignIn = 0xf090;
	static const int faSignOut = 0xf08b;
	static const int faSignal = 0xf012;
	static const int faSimplybuilt = 0xf215;
	static const int faSitemap = 0xf0e8;
	static const int faSkyatlas = 0xf216;
	static const int faSkype = 0xf17e;
	static const int faSlack = 0xf198;
	static const int faSliders = 0xf1de;
	static const int faSlideshare = 0xf1e7;
	static const int faSmileO = 0xf118;
	static const int faSoccerBallO = 0xf1e3;
	static const int faSort = 0xf0dc;
	static const int faSortAlphaAsc = 0xf15d;
	static const int faSortAlphaDesc = 0xf15e;
	static const int faSortAmountAsc = 0xf160;
	static const int faSortAmountDesc = 0xf161;
	static const int faSortAsc = 0xf0de;
	static const int faSortDesc = 0xf0dd;
	static const int faSortDown = 0xf0dd;
	static const int faSortNumericAsc = 0xf162;
	static const int faSortNumericDesc = 0xf163;
	static const int faSortUp = 0xf0de;
	static const int faSoundcloud = 0xf1be;
	static const int faSpaceShuttle = 0xf197;
	static const int faSpinner = 0xf110;
	static const int faSpoon = 0xf1b1;
	static const int faSpotify = 0xf1bc;
	static const int faSquare = 0xf0c8;
	static const int faSquareO = 0xf096;
	static const int faStackExchange = 0xf18d;
	static const int faStackOverflow = 0xf16c;
	static const int faStar = 0xf005;
	static const int faStarHalf = 0xf089;
	static const int faStarHalfEmpty = 0xf123;
	static const int faStarHalfFull = 0xf123;
	static const int faStarHalfO = 0xf123;
	static const int faStarO = 0xf006;
	static const int faSteam = 0xf1b6;
	static const int faSteamSquare = 0xf1b7;
	static const int faStepBackward = 0xf048;
	static const int faStepForward = 0xf051;
	static const int faStethoscope = 0xf0f1;
	static const int faStickyNote = 0xf249;
	static const int faStickyNoteO = 0xf24a;
	static const int faStop = 0xf04d;
	static const int faStopCircle = 0xf28d;
	static const int faStopCircleO = 0xf28e;
	static const int faStreetView = 0xf21d;
	static const int faStrikethrough = 0xf0cc;
	static const int faStumbleupon = 0xf1a4;
	static const int faStumbleuponCircle = 0xf1a3;
	static const int faSubscript = 0xf12c;
	static const int faSubway = 0xf239;
	static const int faSuitcase = 0xf0f2;
	static const int faSunO = 0xf185;
	static const int faSuperscript = 0xf12b;
	static const int faSupport = 0xf1cd;
	static const int faTable = 0xf0ce;
	static const int faTablet = 0xf10a;
	static const int faTachometer = 0xf0e4;
	static const int faTag = 0xf02b;
	static const int faTags = 0xf02c;
	static const int faTasks = 0xf0ae;
	static const int faTaxi = 0xf1ba;
	static const int faTelevision = 0xf26c;
	static const int faTencentWeibo = 0xf1d5;
	static const int faTerminal = 0xf120;
	static const int faTextHeight = 0xf034;
	static const int faTextWidth = 0xf035;
	static const int faTh = 0xf00a;
	static const int faThLarge = 0xf009;
	static const int faThList = 0xf00b;
	static const int faThumbTack = 0xf08d;
	static const int faThumbsDown = 0xf165;
	static const int faThumbsODown = 0xf088;
	static const int faThumbsOUp = 0xf087;
	static const int faThumbsUp = 0xf164;
	static const int faTicket = 0xf145;
	static const int faTimes = 0xf00d;
	static const int faTimesCircle = 0xf057;
	static const int faTimesCircleO = 0xf05c;
	static const int faTint = 0xf043;
	static const int faToggleDown = 0xf150;
	static const int faToggleLeft = 0xf191;
	static const int faToggleOff = 0xf204;
	static const int faToggleOn = 0xf205;
	static const int faToggleRight = 0xf152;
	static const int faToggleUp = 0xf151;
	static const int faTrademark = 0xf25c;
	static const int faTrain = 0xf238;
	static const int faTransgender = 0xf224;
	static const int faTransgenderAlt = 0xf225;
	static const int faTrash = 0xf1f8;
	static const int faTrashO = 0xf014;
	static const int faTree = 0xf1bb;
	static const int faTrello = 0xf181;
	static const int faTripadvisor = 0xf262;
	static const int faTrophy = 0xf091;
	static const int faTruck = 0xf0d1;
	static const int faTry = 0xf195;
	static const int faTty = 0xf1e4;
	static const int faTumblr = 0xf173;
	static const int faTumblrSquare = 0xf174;
	static const int faTurkishLira = 0xf195;
	static const int faTv = 0xf26c;
	static const int faTwitch = 0xf1e8;
	static const int faTwitter = 0xf099;
	static const int faTwitterSquare = 0xf081;
	static const int faUmbrella = 0xf0e9;
	static const int faUnderline = 0xf0cd;
	static const int faUndo = 0xf0e2;
	static const int faUniversity = 0xf19c;
	static const int faUnlink = 0xf127;
	static const int faUnlock = 0xf09c;
	static const int faUnlockAlt = 0xf13e;
	static const int faUnsorted = 0xf0dc;
	static const int faUpload = 0xf093;
	static const int faUsb = 0xf287;
	static const int faUsd = 0xf155;
	static const int faUser = 0xf007;
	static const int faUserMd = 0xf0f0;
	static const int faUserPlus = 0xf234;
	static const int faUserSecret = 0xf21b;
	static const int faUserTimes = 0xf235;
	static const int faUsers = 0xf0c0;
	static const int faVenus = 0xf221;
	static const int faVenusDouble = 0xf226;
	static const int faVenusMars = 0xf228;
	static const int faViacoin = 0xf237;
	static const int faVideoCamera = 0xf03d;
	static const int faVimeo = 0xf27d;
	static const int faVimeoSquare = 0xf194;
	static const int faVine = 0xf1ca;
	static const int faVk = 0xf189;
	static const int faVolumeDown = 0xf027;
	static const int faVolumeOff = 0xf026;
	static const int faVolumeUp = 0xf028;
	static const int faWarning = 0xf071;
	static const int faWechat = 0xf1d7;
	static const int faWeibo = 0xf18a;
	static const int faWeixin = 0xf1d7;
	static const int faWhatsapp = 0xf232;
	static const int faWheelchair = 0xf193;
	static const int faWifi = 0xf1eb;
	static const int faWikipediaW = 0xf266;
	static const int faWindows = 0xf17a;
	static const int faWon = 0xf159;
	static const int faWordpress = 0xf19a;
	static const int faWrench = 0xf0ad;
	static const int faXing = 0xf168;
	static const int faXingSquare = 0xf169;
	static const int faYCombinator = 0xf23b;
	static const int faYCombinatorSquare = 0xf1d4;
	static const int faYahoo = 0xf19e;
	static const int faYc = 0xf23b;
	static const int faYcSquare = 0xf1d4;
	static const int faYelp = 0xf1e9;
	static const int faYen = 0xf157;
	static const int faYoutube = 0xf167;
	static const int faYoutubePlay = 0xf16a;
	static const int faYoutubeSquare = 0xf166;
};

#endif  // FONTAWESOME_H_INCLUDED


