#include "ObjectToolbox.h"
USING_NS_CC;

// thank you smjs for that reddit post: https://www.reddit.com/r/geometrydash/comments/s80t96/how_50_of_gd_is_duplicate_code/
#define CREATE_OBJ(filename, index) \
	key = CCString::createWithFormat("%i", index); \
	ObjectToolbox::m_objects->setObject(key, filename); \
	ObjectToolbox::m_keys->setObject(CCString::create(filename), index);

// the whole macro mess makes this function expand to around 2,800 lines in the end of it all
bool ObjectToolbox::init()
{
	m_objects = CCDictionary::create();
	m_objects->retain();

	m_keys = CCDictionary::create();
	m_keys->retain();

	CCString* key;
	// object names taken from https://flowvix.github.io/gd-info-explorer/ids 
// ID/frame pairs recovered from ObjectToolbox::init in the original 1.71 x86 ELF.
	CREATE_OBJ("square_01_001.png", 1);
	CREATE_OBJ("square_02_001.png", 2);
	CREATE_OBJ("square_03_001.png", 3);
	CREATE_OBJ("square_04_001.png", 4);
	CREATE_OBJ("square_05_001.png", 5);
	CREATE_OBJ("square_06_001.png", 6);
	CREATE_OBJ("square_07_001.png", 7);
	CREATE_OBJ("spike_01_001.png", 8);
	CREATE_OBJ("pit_01_001.png", 9);
	CREATE_OBJ("portal_01_front_001.png", 10);
	CREATE_OBJ("portal_02_front_001.png", 11);
	CREATE_OBJ("portal_03_front_001.png", 12);
	CREATE_OBJ("portal_04_front_001.png", 13);
	CREATE_OBJ("rod_01_001.png", 15);
	CREATE_OBJ("rod_02_001.png", 16);
	CREATE_OBJ("rod_03_001.png", 17);
	CREATE_OBJ("d_spikes_01_001.png", 18);
	CREATE_OBJ("d_spikes_02_001.png", 19);
	CREATE_OBJ("d_spikes_03_001.png", 20);
	CREATE_OBJ("d_spikes_04_001.png", 21);
	CREATE_OBJ("edit_eeNoneBtn_001.png", 22);
	CREATE_OBJ("edit_eeFBBtn_001.png", 23);
	CREATE_OBJ("edit_eeFTBtn_001.png", 24);
	CREATE_OBJ("edit_eeFLBtn_001.png", 25);
	CREATE_OBJ("edit_eeFRBtn_001.png", 26);
	CREATE_OBJ("edit_eeSUBtn_001.png", 27);
	CREATE_OBJ("edit_eeSDBtn_001.png", 28);
	CREATE_OBJ("edit_eTintBGBtn_001.png", 29);
	CREATE_OBJ("edit_eTintGBtn_001.png", 30);
	CREATE_OBJ("edit_eStartPosBtn_001.png", 31);
	CREATE_OBJ("edit_eGhostEBtn_001.png", 32);
	CREATE_OBJ("edit_eGhostDBtn_001.png", 33);
	CREATE_OBJ("edit_eLevelEndBtn_001.png", 34);
	CREATE_OBJ("bump_01_001.png", 35);
	CREATE_OBJ("ring_01_001.png", 36);
	CREATE_OBJ("spike_02_001.png", 39);
	CREATE_OBJ("plank_01_001.png", 40);
	CREATE_OBJ("chain_01_001.png", 41);
	CREATE_OBJ("edit_eBGEOn_001.png", 42);
	CREATE_OBJ("edit_eBGEOff_001.png", 43);
	CREATE_OBJ("portal_05_front_001.png", 45);
	CREATE_OBJ("portal_06_front_001.png", 46);
	CREATE_OBJ("portal_07_front_001.png", 47);
	CREATE_OBJ("d_cloud_01_001.png", 48);
	CREATE_OBJ("d_cloud_02_001.png", 49);
	CREATE_OBJ("d_ball_01_001.png", 50);
	CREATE_OBJ("d_ball_02_001.png", 51);
	CREATE_OBJ("d_ball_03_001.png", 52);
	CREATE_OBJ("d_ball_04_001.png", 53);
	CREATE_OBJ("d_ball_05_001.png", 54);
	CREATE_OBJ("edit_eeFABtn_001.png", 55);
	CREATE_OBJ("edit_eeFALBtn_001.png", 56);
	CREATE_OBJ("edit_eeFARBtn_001.png", 57);
	CREATE_OBJ("edit_eeFRHBtn_001.png", 58);
	CREATE_OBJ("edit_eeFRHInvBtn_001.png", 59);
	CREATE_OBJ("d_ball_06_001.png", 60);
	CREATE_OBJ("pit_04_001.png", 61);
	CREATE_OBJ("square_b_01_001.png", 62);
	CREATE_OBJ("square_b_02_001.png", 63);
	CREATE_OBJ("square_b_03_001.png", 64);
	CREATE_OBJ("square_b_04_001.png", 65);
	CREATE_OBJ("square_b_05_001.png", 66);
	CREATE_OBJ("gravbump_01_001.png", 67);
	CREATE_OBJ("square_b_06_001.png", 68);
	CREATE_OBJ("square_c_01_001.png", 69);
	CREATE_OBJ("square_c_02_001.png", 70);
	CREATE_OBJ("square_c_03_001.png", 71);
	CREATE_OBJ("square_c_04_001.png", 72);
	CREATE_OBJ("square_c_05_001.png", 73);
	CREATE_OBJ("square_c_06_001.png", 74);
	CREATE_OBJ("square_c_07_001.png", 75);
	CREATE_OBJ("square_d_01_001.png", 76);
	CREATE_OBJ("square_d_02_001.png", 77);
	CREATE_OBJ("square_d_03_001.png", 78);
	CREATE_OBJ("square_d_05_001.png", 80);
	CREATE_OBJ("square_d_06_001.png", 81);
	CREATE_OBJ("square_d_07_001.png", 82);
	CREATE_OBJ("square_08_001.png", 83);
	CREATE_OBJ("gravring_01_001.png", 84);
	CREATE_OBJ("d_cogwheel_01_001.png", 85);
	CREATE_OBJ("d_cogwheel_02_001.png", 86);
	CREATE_OBJ("d_cogwheel_03_001.png", 87);
	CREATE_OBJ("sawblade_01_001.png", 88);
	CREATE_OBJ("sawblade_02_001.png", 89);
	CREATE_OBJ("square_e_01_001.png", 90);
	CREATE_OBJ("square_e_02_001.png", 91);
	CREATE_OBJ("square_e_03_001.png", 92);
	CREATE_OBJ("square_e_04_001.png", 93);
	CREATE_OBJ("square_e_05_001.png", 94);
	CREATE_OBJ("square_e_06_001.png", 95);
	CREATE_OBJ("square_e_07_001.png", 96);
	CREATE_OBJ("d_cogwheel_04_001.png", 97);
	CREATE_OBJ("sawblade_03_001.png", 98);
	CREATE_OBJ("portal_08_front_001.png", 99);
	CREATE_OBJ("portal_09_front_001.png", 101);
	CREATE_OBJ("spike_03_001.png", 103);
	CREATE_OBJ("edit_eTintLBtn_001.png", 104);
	CREATE_OBJ("edit_eTintObjBtn_001.png", 105);
	CREATE_OBJ("d_02_chain_01_001.png", 106);
	CREATE_OBJ("d_02_chain_02_001.png", 107);
	CREATE_OBJ("d_chain_02_001.png", 110);
	CREATE_OBJ("portal_10_front_001.png", 111);
	CREATE_OBJ("portal_10_back_001.png", 112);
	CREATE_OBJ("d_brick_01_001.png", 113);
	CREATE_OBJ("d_brick_02_001.png", 114);
	CREATE_OBJ("d_brick_03_001.png", 115);
	CREATE_OBJ("square_f_01_001.png", 116);
	CREATE_OBJ("square_f_02_001.png", 117);
	CREATE_OBJ("square_f_03_001.png", 118);
	CREATE_OBJ("square_f_04_001.png", 119);
	CREATE_OBJ("square_f_05_001.png", 120);
	CREATE_OBJ("square_f_06_001.png", 121);
	CREATE_OBJ("square_f_07_001.png", 122);
	CREATE_OBJ("d_thorn_01_001.png", 123);
	CREATE_OBJ("d_thorn_02_001.png", 124);
	CREATE_OBJ("d_thorn_03_001.png", 125);
	CREATE_OBJ("d_thorn_04_001.png", 126);
	CREATE_OBJ("d_thorn_05_001.png", 127);
	CREATE_OBJ("d_thorn_06_001.png", 128);
	CREATE_OBJ("d_cloud_03_001.png", 129);
	CREATE_OBJ("d_cloud_04_001.png", 130);
	CREATE_OBJ("d_cloud_05_001.png", 131);
	CREATE_OBJ("d_arrow_01_001.png", 132);
	CREATE_OBJ("d_exmark_01_001.png", 133);
	CREATE_OBJ("d_art_01_001.png", 134);
	CREATE_OBJ("pit_b_01_001.png", 135);
	CREATE_OBJ("d_qmark_01_001.png", 136);
	CREATE_OBJ("d_wheel_01_001.png", 137);
	CREATE_OBJ("d_wheel_02_001.png", 138);
	CREATE_OBJ("d_wheel_03_001.png", 139);
	CREATE_OBJ("bump_03_001.png", 140);
	CREATE_OBJ("ring_03_001.png", 141);
	CREATE_OBJ("secretCoin_01_001.png", 142);
	CREATE_OBJ("brick_02_001.png", 143);
	CREATE_OBJ("invis_spike_01_001.png", 144);
	CREATE_OBJ("invis_spike_03_001.png", 145);
	CREATE_OBJ("invis_square_01_001.png", 146);
	CREATE_OBJ("invis_plank_01_001.png", 147);
	CREATE_OBJ("d_ball_07_001.png", 148);
	CREATE_OBJ("d_ball_08_001.png", 149);
	CREATE_OBJ("d_cross_01_001.png", 150);
	CREATE_OBJ("d_spikeart_01_001.png", 151);
	CREATE_OBJ("d_spikeart_02_001.png", 152);
	CREATE_OBJ("d_spikeart_03_001.png", 153);
	CREATE_OBJ("d_spikewheel_01_001.png", 154);
	CREATE_OBJ("d_spikewheel_02_001.png", 155);
	CREATE_OBJ("d_spikewheel_03_001.png", 156);
	CREATE_OBJ("d_wave_01_001.png", 157);
	CREATE_OBJ("d_wave_02_001.png", 158);
	CREATE_OBJ("d_wave_03_001.png", 159);
	CREATE_OBJ("square_g_01_001.png", 160);
	CREATE_OBJ("square_g_02_001.png", 161);
	CREATE_OBJ("square_g_03_001.png", 162);
	CREATE_OBJ("square_g_04_001.png", 163);
	CREATE_OBJ("square_g_05_001.png", 164);
	CREATE_OBJ("square_g_06_001.png", 165);
	CREATE_OBJ("square_g_07_001.png", 166);
	CREATE_OBJ("square_g_08_001.png", 167);
	CREATE_OBJ("square_g_09_001.png", 168);
	CREATE_OBJ("square_g_10_001.png", 169);
	CREATE_OBJ("square_h_01_001.png", 170);
	CREATE_OBJ("square_h_02_001.png", 171);
	CREATE_OBJ("square_h_03_001.png", 172);
	CREATE_OBJ("square_h_04_001.png", 173);
	CREATE_OBJ("square_h_05_001.png", 174);
	CREATE_OBJ("square_h_06_001.png", 175);
	CREATE_OBJ("square_h_07_001.png", 176);
	CREATE_OBJ("iceSpike_01_001.png", 177);
	CREATE_OBJ("iceSpike_02_001.png", 178);
	CREATE_OBJ("iceSpike_03_001.png", 179);
	CREATE_OBJ("d_cartwheel_01_001.png", 180);
	CREATE_OBJ("d_cartwheel_02_001.png", 181);
	CREATE_OBJ("d_cartwheel_03_001.png", 182);
	CREATE_OBJ("blade_b_01_001.png", 183);
	CREATE_OBJ("blade_b_02_001.png", 184);
	CREATE_OBJ("blade_b_03_001.png", 185);
	CREATE_OBJ("blade_01_001.png", 186);
	CREATE_OBJ("blade_02_001.png", 187);
	CREATE_OBJ("blade_03_001.png", 188);
	CREATE_OBJ("d_art_02_001.png", 190);
	CREATE_OBJ("fakeSpike_01_001.png", 191);
	CREATE_OBJ("square_h_08_001.png", 192);
	CREATE_OBJ("square_g_11_001.png", 193);
	CREATE_OBJ("square_h_09_001.png", 194);
	CREATE_OBJ("square_01_small_001.png", 195);
	CREATE_OBJ("plank_01_small_001.png", 196);
	CREATE_OBJ("square_h_10_001.png", 197);
	CREATE_OBJ("fakeSpike_02_001.png", 198);
	CREATE_OBJ("fakeSpike_03_001.png", 199);
	CREATE_OBJ("boost_01_001.png", 200);
	CREATE_OBJ("boost_02_001.png", 201);
	CREATE_OBJ("boost_03_001.png", 202);
	CREATE_OBJ("boost_04_001.png", 203);
	CREATE_OBJ("invis_plank_01_small_001.png", 204);
	CREATE_OBJ("invis_spike_02_001.png", 205);
	CREATE_OBJ("invis_square_01_small_001.png", 206);
	CREATE_OBJ("lightsquare_01_01_001.png", 207);
	CREATE_OBJ("lightsquare_01_02_001.png", 208);
	CREATE_OBJ("lightsquare_01_03_001.png", 209);
	CREATE_OBJ("lightsquare_01_04_001.png", 210);
	CREATE_OBJ("lightsquare_01_05_001.png", 211);
	CREATE_OBJ("lightsquare_01_06_001.png", 212);
	CREATE_OBJ("lightsquare_01_07_001.png", 213);
	CREATE_OBJ("colorPlank_01_001.png", 215);
	CREATE_OBJ("colorSpike_01_001.png", 216);
	CREATE_OBJ("colorSpike_02_001.png", 217);
	CREATE_OBJ("colorSpike_03_001.png", 218);
	CREATE_OBJ("colorPlank_01_small_001.png", 219);
	CREATE_OBJ("colorSquare_01_small_001.png", 220);
	CREATE_OBJ("edit_eTintColObjBtn_001.png", 221);
	CREATE_OBJ("d_roundCloud_01_001.png", 222);
	CREATE_OBJ("d_roundCloud_02_001.png", 223);
	CREATE_OBJ("d_roundCloud_03_001.png", 224);
	CREATE_OBJ("d_swirve_01_001.png", 225);
	CREATE_OBJ("d_swirve_02_001.png", 226);
	CREATE_OBJ("d_bar_01_001.png", 227);
	CREATE_OBJ("d_bar_02_001.png", 228);
	CREATE_OBJ("d_bar_03_001.png", 229);
	CREATE_OBJ("d_bar_04_001.png", 230);
	CREATE_OBJ("d_smallbar_01_001.png", 231);
	CREATE_OBJ("d_smallbar_02_001.png", 232);
	CREATE_OBJ("d_square_03_01_001.png", 233);
	CREATE_OBJ("d_square_03_02_001.png", 234);
	CREATE_OBJ("d_square_03_03_001.png", 235);
	CREATE_OBJ("d_circle_01_001.png", 236);
	CREATE_OBJ("d_link_01_001.png", 237);
	CREATE_OBJ("d_link_02_001.png", 238);
	CREATE_OBJ("d_link_03_001.png", 239);
	CREATE_OBJ("d_link_04_001.png", 240);
	CREATE_OBJ("d_link_05_001.png", 241);
	CREATE_OBJ("d_bar_07_001.png", 242);
	CREATE_OBJ("pit_04_02_001.png", 243);
	CREATE_OBJ("pit_04_03_001.png", 244);
	CREATE_OBJ("square_f_brick01_001.png", 245);
	CREATE_OBJ("square_f_brick02_001.png", 246);
	CREATE_OBJ("lightsquare_02_01_001.png", 247);
	CREATE_OBJ("lightsquare_02_02_001.png", 248);
	CREATE_OBJ("lightsquare_02_03_001.png", 249);
	CREATE_OBJ("lightsquare_02_04_001.png", 250);
	CREATE_OBJ("lightsquare_02_05_001.png", 251);
	CREATE_OBJ("lightsquare_02_06_001.png", 252);
	CREATE_OBJ("lightsquare_02_07_001.png", 253);
	CREATE_OBJ("lightsquare_02_08_001.png", 254);
	CREATE_OBJ("lightsquare_03_01_001.png", 255);
	CREATE_OBJ("lightsquare_03_02_001.png", 256);
	CREATE_OBJ("lightsquare_03_03_001.png", 257);
	CREATE_OBJ("lightsquare_03_04_001.png", 258);
	CREATE_OBJ("lightsquare_03_05_001.png", 259);
	CREATE_OBJ("lightsquare_03_06_001.png", 260);
	CREATE_OBJ("lightsquare_03_07_001.png", 261);
	CREATE_OBJ("lightsquare_04_01_001.png", 263);
	CREATE_OBJ("lightsquare_04_02_001.png", 264);
	CREATE_OBJ("lightsquare_04_03_001.png", 265);
	CREATE_OBJ("lightsquare_04_05_001.png", 266);
	CREATE_OBJ("lightsquare_04_06_001.png", 267);
	CREATE_OBJ("lightsquare_04_07_001.png", 268);
	CREATE_OBJ("lightsquare_05_01_001.png", 269);
	CREATE_OBJ("lightsquare_05_02_001.png", 270);
	CREATE_OBJ("lightsquare_05_03_001.png", 271);
	CREATE_OBJ("lightsquare_05_04_001.png", 272);
	CREATE_OBJ("lightsquare_05_05_001.png", 273);
	CREATE_OBJ("lightsquare_05_06_001.png", 274);
	CREATE_OBJ("lightsquare_05_07_001.png", 275);
	CREATE_OBJ("lightsquare_05_brick02_001.png", 277);
	CREATE_OBJ("lightsquare_05_brick03_001.png", 278);
	CREATE_OBJ("d_square_01_001.png", 279);
	CREATE_OBJ("d_square_02_001.png", 280);
	CREATE_OBJ("d_square_04_001.png", 281);
	CREATE_OBJ("d_square_05_001.png", 282);
	CREATE_OBJ("d_smallbar_03_001.png", 283);
	CREATE_OBJ("d_smallbar_04_001.png", 284);
	CREATE_OBJ("d_smallbar_05_001.png", 285);
#pragma endregion Objects
	return true;
}

ObjectToolbox* ObjectToolbox::sharedState()
{
	static ObjectToolbox* pObjectToolbox = NULL;
	if (!pObjectToolbox)
	{
		pObjectToolbox = new ObjectToolbox();
		pObjectToolbox->init();
	}

	return pObjectToolbox;
}

CCDictionary* ObjectToolbox::stringSetupToDict(std::string str)
{
    CCDictionary* dict = CCDictionary::create();
    
    std::stringstream strStream(str);
    std::string currentKey;
    std::string keyID;
    
    unsigned int i = 0;
    while(getline(strStream, currentKey, ',')){
        
        if(i % 2 == 0) keyID = currentKey;
		else dict->setObject(CCString::create(currentKey.c_str()), keyID);
        i++;
    }
    
    return dict;
}

char const* ObjectToolbox::keyToFrame(char const* key)
{
	return m_keys->valueForKey(atoi(key))->getCString();
}