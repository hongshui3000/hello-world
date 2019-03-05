#include "Front.h"




#define FRONT_OVERTIME       10000                //ǰ��״̬��ʱ

typedef void (*FrontState_t)(void);              //ǰ��״̬����
FrontState_t g_front_state;                      //ǰ��״̬
u32 g_front_inited;                              //��ǰ״̬�Ƿ��ʼ����
u32 g_front_stamp;                               //ǰ�峬ʱʱ���
u32 g_front_error_num;                           //��������/ָ��/���ŵĴ���

u32 g_config_stamp;                              //���ü�ʱ���
u32 g_config_last;                               //���ü������״̬
u32 g_config_triggered;                          //���ü��Ƿ��Ѿ�����

    

ParamAwake_t g_front_param;//����״̬�Ĳ����ṹ

//===========================ǰ��״̬���ֲ�����===================================//


//===========================ǰ��״̬��״̬����===================================//
static void FrontSleep(void);                    //˯��״̬
static void FrontAwake(void);                    //����״̬,������֤��������/ָ��/��
static void FrontUnlock(void);                   //����״̬
static void FrontAlarm(void);                    //����״̬
static void FrontVerifyAdmin(void);              //��֤����Ա״̬
static void FrontMenuTop(void);                  //�������ò˵�״̬
static void FrontMenuAddUser(void);              //����û�
static void FrontMenuDeleteUser(void);           //ɾ���û�
static void FrontAddAdmin(void);                 //��ӹ���Ա
static void FrontAddNormalUser(void);            //���һ���û�
static void FrontDeleteAllUser(void);            //ɾ�������û�
static void FrontDeleteNormalUser(void);         //ɾ��һ���û�
static void FrontAddNormalKeyUser(void);         //���һ�������û�
static void FrontAddNormalFingerUser(void);      //���һ��ָ���û�
static void FrontAddNormalRfidUser(void);        //���һ�㿨�û�




void ChangeNumtoString(void)
{
	  uint8_t cnt;
		uint8_t numlen = g_user_config.password_num_len;
	  for(cnt = 0;cnt < numlen;cnt ++)
		{
				if(cnt < 8)
				{
					g_user_config.passwordnum[numlen - 1 - cnt] = (g_user_config.password_unlock_lo >>(4*cnt)&0xF);
				}
				else
				{
					g_user_config.passwordnum[numlen - 1 - cnt] = (g_user_config.password_unlock_hi >>(4*(cnt -8))&0xF);
				}
		}
		return;
}
// 0 : fail
// 1 : pass
uint8_t CheckKey(u8 * checkbuffer,u8 buflen,u8 * password,u8 passwordlen)
{
		uint8_t result,cnt_t,offset;
		result = 0;
	  if(buflen < passwordlen)
		{
				return 0;
		}
		offset  = buflen - passwordlen;
		for(cnt_t = 0;cnt_t <=offset;cnt_t ++)
		{
			if(memcmp((const void *)(checkbuffer + cnt_t),(const void *)password,passwordlen)== 0)
			{
				result = 1;
				break;
			}
		}
		return result;
}
//˯��״̬����������Touch�ͻ���
static void FrontSleep(void)
{
    if(g_front_inited)
		{
        if (FrontGetKeyTouch()||FrontGetFingerTouch())
				{
					  //g_hardware_input.key_touch = 0;
					  g_hardware_input.key = KEY_LED_NONE;
            g_front_state = FrontAwake;
            g_front_inited = 0;
					  
        }

    }
		else
		{
        ScreenClear();
        g_front_error_num = 0;

        g_front_inited = 1;
			  debugPrint(" Front Sleep\n");
			  g_hardware_input.key_touch = 0;
			  g_hardware_input.finger_touch = 0;
        //qDebug() << "FrontSleep";
    }
}

static u32 FrontGetStamp(void)
{
	return  systemTick();
}
static u32 FrontCalcElapsed(u32 tick)
{
	  uint32_t temptime;
		temptime = systemTick();
	  if(temptime >= g_front_stamp)
		{
			 temptime =  temptime - g_front_stamp;
		}
		else
		{
			 temptime = 0xFFFFFFFF - g_front_stamp + temptime;
		}
		return temptime;
}


//����״̬���������뿪������/������ָ/ˢRFID����
static void FrontAwake(void)
{
    ParamAwake_t *param = (ParamAwake_t *)&g_front_param;
    s32 id, idx;
    KeyLed_t key;
    if(g_front_inited)
		{
        //��������
				key = FrontGetKey();
        if (key == KEY_LED_NONE)
				{
            param->key_last = key;
        }
				else if (key != param->key_last)
				{
            g_front_stamp = FrontGetStamp();     //����ʱ���
            param->key_last = key;
            VoicePlay(VOICE_BEEP);
            if (key == KEY_LED_HASH)
						{            //#��
                if (param->password_len > 0)
								{  
										if(UserFindAdmin()||UserFindKeyUser())
										{
											  g_front_state = FrontUnlock;
											  g_front_inited = 0;
											
										}
										else
										{
											  //�Ƿ�����
											  g_front_state = FrontAlarm;
											  g_front_inited = 0;
										}
										 memset(param->password_buff,0,256);
										 param->password_len = 0;
							}
							else
							{
								  if(g_user_config.admin_write_index == 0)
									{
										 g_front_state = FrontMenuTop;
										 g_front_inited = 0;
									}
									else
									{
											g_front_state = FrontVerifyAdmin;
										  g_front_inited = 0;
									}
							}
								
            }
					  else if (key == KEY_LED_STAR)
						{   //*��
                //�������
							  memset(param->password_buff,0,256);
							  param->password_len = 0;
							
							 debugPrint(" \r\n clear input \r\n");
                //param->password_lo = 0xFFFFFFFF;
                //param->password_hi = 0xFFFFFFFF;
            }
						else
						{                               //���ּ�
                //����һλ
							  if(g_user_config.admin_write_index == 0)
								{
										g_front_state = FrontUnlock;
										g_front_inited = 0;
								}
								else
								{
										param->password_buff[param->password_len] = key;
										param->password_len ++;
										debugPrint(" %d",key);
								}
            }
        }
				if(FrontGetFingerTouch() == 1)//finger pressed
				{
						if(g_user_config.admin_write_index == 0)
						{
								g_front_state = FrontUnlock;
								g_front_inited = 0;
						}
						else
						{
							//ƥ��
							
						}
				}
				else
        //��������
        {
            id = FrontGetRfid();
            if (id >= 0)
						{
                g_front_stamp = FrontGetStamp(); //����ʱ���
                idx = UserFindRfid(id);
                if (idx >= 0)
								{                   //����
                    g_front_state = FrontUnlock;
                    g_front_inited = 0;
                }
								else
								{                           //�Ƿ�����
                    g_front_state = FrontAlarm;
                    g_front_inited = 0;
                }
            }
        }
        //��ʱ����
        if (FrontCalcElapsed(g_front_stamp) >= FRONT_OVERTIME)
				{
            g_front_state = FrontSleep;
            g_front_inited = 0;
					  
        }
    }
		else
		{
        ScreenShowLogo();
        VoicePlay(VOICE_WELCOME);

        param->key_last = KEY_LED_NONE;
			  memset(param->password_buff,0,256);
				param->password_len = 0;
        //param->password_lo = 0xFFFFFFFF;
        //param->password_hi = 0xFFFFFFFF;

        g_front_inited = 1;
        g_front_stamp = FrontGetStamp();
			  //debugPrint(" Front Awake\r\n");
			  debugPrint(" *********************************\r\n");
			  debugPrint(" Welcome   \r\n");
			  debugPrint(" *********************************\r\n");
        //qDebug() << "FrontAwake";
    }
}
//����״̬
static void FrontUnlock(void)
{
    if(g_front_inited)
		{

        //��ʱ����
        if (FrontCalcElapsed(g_front_stamp) >= FRONT_OVERTIME)
				{
            g_front_state = FrontSleep;
            g_front_inited = 0;
        }
    }
		else
		{
        g_front_error_num = 0;

        g_front_inited = 1;
        g_front_stamp = FrontGetStamp();
			  debugPrint(" Front Ulock\n");
        //qDebug() << "FrontUnlock";
    }
}
//����״̬
static void FrontAlarm(void)
{
	   KeyLed_t key;
    if(g_front_inited)
		{
						   //
			  key = FrontGetKey();
        if (key == KEY_LED_NONE)
				{
            g_front_state = FrontAwake;
            g_front_inited = 0;
        }
			  
        if (FrontCalcElapsed(g_front_stamp) >= FRONT_OVERTIME)
				{
            if (g_front_error_num >= 5){

            }else{
                g_front_state = FrontSleep;
                g_front_inited = 0;
            }
        }
    }
		else
		{
        g_front_error_num++;

        g_front_inited = 1;
        g_front_stamp = FrontGetStamp();
			  debugPrint(" Password Error\n");
        //qDebug() << "FrontAlarm";
    }
}
//��֤����Ա״̬
static void FrontVerifyAdmin(void)
{
    ParamAwake_t *param = (ParamAwake_t *)&g_front_param;
    KeyLed_t key;
    if(g_front_inited)
		{
        //��������
        key = FrontGetKey();
        if (key == KEY_LED_NONE)
				{
            param->key_last = key;
        }
				else if (key != param->key_last)
				{
            g_front_stamp = FrontGetStamp();     //����ʱ���
            param->key_last = key;
            VoicePlay(VOICE_BEEP);
            if (key == KEY_LED_HASH)
						{            //#��
                if (param->password_len > 0 )
								{   
									  if(UserFindAdmin())
										{
											  g_front_state = FrontMenuTop;
											  g_front_inited = 0;
											
										}
										else
										{
											  //�Ƿ�����
											  g_front_state = FrontAlarm;
											  g_front_inited = 0;
										}

                }
								memset(param->password_buff,0,256);
								param->password_len = 0;
            }
						else if (key == KEY_LED_STAR)
						{      //*��
                //�������
							  if(param->password_len > 0)
								{
									  memset(param->password_buff,0,256);
										param->password_len = 0;
									  debugPrint(" \r\n clear input \r\n");
								}
								else
								{
									 g_front_state = FrontAwake;
									 g_front_inited = 0;
								}
							  
            }
						else
						{                               //���ּ�
                //����һλ
							  param->password_buff[param->password_len] = key;
							  param->password_len ++;
							  debugPrint(" %d",key);
            }
        }
        //��ʱ����
        if (FrontCalcElapsed(g_front_stamp) >= FRONT_OVERTIME)
				{
            g_front_state = FrontSleep;
            g_front_inited = 0;
        }
    }
		else
		{
        param->key_last = KEY_LED_NONE;
			  memset(param->password_buff,0,256);
				param->password_len = 0;

        g_front_inited = 1;
        g_front_stamp = FrontGetStamp();
			  debugPrint(" please input admin password \n");
        //qDebug() << "FrontAdmin";
    }
}
//�������ò˵�״̬
static void FrontMenuTop(void)
{
		ParamAwake_t *param = (ParamAwake_t *)&g_front_param;
		KeyLed_t key;
    if(g_front_inited)
		{
				//��������
        key = FrontGetKey();
        if (key == KEY_LED_NONE)
				{
            param->key_last = key;
        }
				else if (key != param->key_last)
				{
            g_front_stamp = FrontGetStamp();     //����ʱ���
            param->key_last = key;
            VoicePlay(VOICE_BEEP);
            if (key == KEY_LED_1)
						{      
								//1��
								g_front_state = FrontMenuAddUser;
								g_front_inited = 0;

            }
						else if (key == KEY_LED_2)
						{   //2��
                //ɾ���û�
								g_front_state = FrontMenuDeleteUser;
								g_front_inited = 0;
            }
						else if(key == KEY_LED_STAR) //* key
						{                            //������һ��Ŀ¼
                g_front_state = FrontAwake;
                g_front_inited = 0;
            }
        }
        //��ʱ����
        if (FrontCalcElapsed(g_front_stamp) >= FRONT_OVERTIME)
				{
            g_front_state = FrontSleep;
            g_front_inited = 0;
        }
    }
		else
		{
        g_front_inited = 1;
        g_front_stamp = FrontGetStamp();
				//debugPrint(" Front Menu \r\n");
			  debugPrint(" *********************************\r\n");
			  debugPrint(" Press 1 :add user\r\n");
			  debugPrint(" Press 2 :delete user\r\n");
			  debugPrint(" *********************************\r\n");
        //qDebug() << "FrontMenu";
    }
}



//����û����ò˵�״̬
static void FrontMenuAddUser(void)
{
		ParamAwake_t *param = (ParamAwake_t *)&g_front_param;
		KeyLed_t key;
    if(g_front_inited)
		{
				//��������
        key = FrontGetKey();
        if (key == KEY_LED_NONE)
				{
            param->key_last = key;
        }
				else if (key != param->key_last)
				{
            g_front_stamp = FrontGetStamp();     //����ʱ���
            param->key_last = key;
            VoicePlay(VOICE_BEEP);
            if (key == KEY_LED_1)
						{      
								//1��
								g_front_state = FrontAddAdmin;
								g_front_inited = 0;

            }
						else if (key == KEY_LED_2)
						{   //2��
                //�������
								g_front_state = FrontAddNormalUser;
								g_front_inited = 0;
            }
						else if(key == KEY_LED_STAR) //* key
						{                            //������һ��Ŀ¼
                g_front_state = FrontMenuTop;
                g_front_inited = 0;
            }
        }
        //��ʱ����
        if (FrontCalcElapsed(g_front_stamp) >= FRONT_OVERTIME)
				{
            g_front_state = FrontSleep;
            g_front_inited = 0;
        }
    }
		else
		{
        g_front_inited = 1;
        g_front_stamp = FrontGetStamp();
			  debugPrint(" *********************************\r\n");
			  debugPrint(" Press 1 :add admin\r\n");
			  debugPrint(" Press 2 :add normal user\r\n");
			  debugPrint(" *********************************\r\n");
        //qDebug() << "FrontMenu";
    }
}


//ɾ���û����ò˵�״̬
static void FrontMenuDeleteUser(void)
{
		ParamAwake_t *param = (ParamAwake_t *)&g_front_param;
		KeyLed_t key;
    if(g_front_inited)
		{
				//��������
        key = FrontGetKey();
        if (key == KEY_LED_NONE)
				{
            param->key_last = key;
        }
				else if (key != param->key_last)
				{
            g_front_stamp = FrontGetStamp();     //����ʱ���
            param->key_last = key;
            VoicePlay(VOICE_BEEP);
            if (key == KEY_LED_1)
						{      
								//1��
								g_front_state = FrontDeleteAllUser;
								g_front_inited = 0;

            }
						else if (key == KEY_LED_2)
						{   //2��
                //�������
								g_front_state = FrontDeleteNormalUser;
								g_front_inited = 0;
            }
						else if(key == KEY_LED_STAR) //* key
						{                            //������һ��Ŀ¼
                g_front_state = FrontMenuTop;
                g_front_inited = 0;
            }
        }
        //��ʱ����
        if (FrontCalcElapsed(g_front_stamp) >= FRONT_OVERTIME)
				{
            g_front_state = FrontSleep;
            g_front_inited = 0;
        }
    }
		else
		{
        g_front_inited = 1;
        g_front_stamp = FrontGetStamp();
			  debugPrint(" *********************************\r\n");
			  debugPrint(" Press 1 :delete all user\r\n");
			  debugPrint(" Press 2 :delete normal user\r\n");
			  debugPrint(" *********************************\r\n");
        //qDebug() << "FrontMenu";
    }
}


static void FrontDeleteAllUser(void)
{
	  ParamAwake_t *param = (ParamAwake_t *)&g_front_param;
		KeyLed_t key;
    if(g_front_inited)
		{
			  //��������
        key = FrontGetKey();
        if (key == KEY_LED_NONE)
				{
            param->key_last = key;
        }
				else if (key != param->key_last)
				{
            g_front_stamp = FrontGetStamp();     //����ʱ���
            param->key_last = key;
            VoicePlay(VOICE_BEEP);
            if (key == KEY_LED_HASH)
						{            //#��
                if (param->password_len > 0 )
								{   
									  if(UserFindAdmin())
										{
											  g_front_state = FrontMenuDeleteUser;
											  g_front_inited = 0;
											  
											  debugPrint(" delete all user success  \r\n");
											  UserDeleteAllKeyUser();
												UserDeleteAllAdminUser();
										}
										else
										{
											  //�Ƿ�����
											  g_front_state = FrontMenuDeleteUser;
											  g_front_inited = 0;
											  
											  debugPrint(" password error ,delete fail  \r\n");
										}

                }
								memset(param->password_buff,0,256);
								param->password_len = 0;
            }
						else if (key == KEY_LED_STAR)
						{      //*��
                //�������
							  if(param->password_len > 0)
								{
									  memset(param->password_buff,0,256);
										param->password_len = 0;
									  debugPrint(" \r\n clear input \r\n");
								}
								else
								{
									 g_front_state = FrontMenuDeleteUser;
									 g_front_inited = 0;
								}
            }
						else
						{                               //���ּ�
                //����һλ
							  param->password_buff[param->password_len] = key;
							  param->password_len ++;
							  debugPrint(" %d",key);
            }
        }
        //��ʱ����
        if (FrontCalcElapsed(g_front_stamp) >= FRONT_OVERTIME)
				{
            g_front_state = FrontMenuDeleteUser;
            g_front_inited = 0;
        }
    }
		else
		{
        g_front_inited = 1;
        g_front_stamp = FrontGetStamp();
			  debugPrint(" please input admin password \n");
    }
}


static  void FrontDeleteNormalUser(void)
{
	  ParamAwake_t *param = (ParamAwake_t *)&g_front_param;
		KeyLed_t key;
    if(g_front_inited)
		{
			  g_front_state = FrontMenuDeleteUser;
        g_front_inited = 0;
        //��ʱ����
        if (FrontCalcElapsed(g_front_stamp) >= FRONT_OVERTIME)
				{
            g_front_state = FrontMenuDeleteUser;
            g_front_inited = 0;
        }
    }
		else
		{
        g_front_inited = 1;
        g_front_stamp = FrontGetStamp();
			  debugPrint(" delete normal user success\n");
			  //ɾ��ָ��
			  //ɾ��RFID
			  //ɾ������
			  UserDeleteAllKeyUser();
    }
	
	
}
//��ӹ���Ա�û�
static void FrontAddAdmin(void)
{
	  ParamAwake_t *param = (ParamAwake_t *)&g_front_param;
    u32 id, idx;
    KeyLed_t key;
    if(g_front_inited)
		{
        //��������
				key = FrontGetKey();
        if (key == KEY_LED_NONE)
				{
            param->key_last = key;
        }
				else if (key != param->key_last)
				{
            g_front_stamp = FrontGetStamp();     //����ʱ���
            param->key_last = key;
            VoicePlay(VOICE_BEEP);
            if (key == KEY_LED_HASH)
						{            //#��
                if (param->password_len > 0)
								{  
									 if(param->password_len > 12)
									 {
										  debugPrint(" password more than 12 ,fail\r\n");
									 }
									 else
									 {
										   debugPrint(" \r\n input password : ");
										   g_user_config.password_admin_hi = -1;
										   g_user_config.password_admin_lo = -1;
											 for(idx = 0;idx < param->password_len;idx ++)
											 {
													 g_user_config.password_admin_hi = (g_user_config.password_admin_hi << 4) | (g_user_config.password_admin_lo >> 28);
                           g_user_config.password_admin_lo = (g_user_config.password_admin_lo << 4) | (param->password_buff[idx] & 0xF);
												   debugPrint(" %d",param->password_buff[idx]);
											 }
											 g_user_config.verify_mode = 1;
											 g_user_config.password_num_len = param->password_len;
											 AdminSaveConfig();
											 debugPrint(" \r\n add adminer success  \r\n");
									 }
									 g_front_state = FrontMenuAddUser;
									 g_front_inited = 0;
									 memset(param->password_buff,0,256);
									 param->password_len = 0;
									#if 0
									//��֤����
									//������
									if ((param->password_lo == g_user_config.password_unlock_lo) &&
											(param->password_hi == g_user_config.password_unlock_hi))
									{//����
											g_front_state = FrontUnlock;
											g_front_inited = 0;
									}else
									{                       //�Ƿ�����
											g_front_state = FrontAlarm;
											g_front_inited = 0;
									}
									#endif
							}
								
            }
					  else if (key == KEY_LED_STAR)
						{   //*��
                //�������
							  if(param->password_len > 0)
								{
									 memset(param->password_buff,0,256);
							     param->password_len = 0;
									 debugPrint("\r\n clear input password   \r\n ");
								}
								else//������һ��
								{ 
									  g_front_state = FrontMenuAddUser;
								    g_front_inited = 0;
								}
            }
						else
						{                               //���ּ�
                //����һλ
							  param->password_buff[param->password_len] = key;
							  param->password_len ++;
							  debugPrint(" %d",key);
            }
        }
				//��ʱ����
        if (FrontCalcElapsed(g_front_stamp) >= FRONT_OVERTIME)
				{
            g_front_state = FrontSleep;
            g_front_inited = 0;
        }
    }
		else
		{
			  memset(param->password_buff,0,256);
				param->password_len = 0;

        g_front_inited = 1;
        g_front_stamp = FrontGetStamp();

			  debugPrint(" *********************************\r\n");
			  debugPrint(" Please add passrord for admin  \r\n");
			  debugPrint(" *********************************\r\n");
        //qDebug() << "FrontAwake";
    }

	
	
}

//���һ���û�
static void FrontAddNormalUser(void)
{
		ParamAwake_t *param = (ParamAwake_t *)&g_front_param;
    s32 id, idx;
    KeyLed_t key;
    if(g_front_inited)
		{
        //��������
				key = FrontGetKey();
        if (key == KEY_LED_NONE)
				{
            param->key_last = key;
        }
				else if (key != param->key_last)
				{
            g_front_stamp = FrontGetStamp();     //����ʱ���
            param->key_last = key;
            VoicePlay(VOICE_BEEP);
            if (key == KEY_LED_1)
						{    //1��
								g_front_state = FrontAddNormalKeyUser;
								g_front_inited = 0;
            }
					  else if (key == KEY_LED_2)
						{   //2��
                g_front_state = FrontAddNormalFingerUser;
								g_front_inited = 0;
            }
						else if (key == KEY_LED_3)
						{   //3��
                g_front_state = FrontAddNormalRfidUser;
								g_front_inited = 0;
            }
						else if (key == KEY_LED_STAR)
						{   //*��
								g_front_state = FrontMenuAddUser;
								g_front_inited = 0;
            }
        }
				
        //��ʱ����
        if (FrontCalcElapsed(g_front_stamp) >= FRONT_OVERTIME)
				{
            g_front_state = FrontSleep;
            g_front_inited = 0;
					  
        }
    }
		else
		{
			  memset(param->password_buff,0,256);
				param->password_len = 0;

        g_front_inited = 1;
        g_front_stamp = FrontGetStamp();
				if(g_user_config.admin_write_index == 0)
				{
					  debugPrint(" *********************************\r\n");
			      debugPrint(" Please add adminer first  \r\n");
			      debugPrint(" *********************************\r\n");
					  g_front_state = FrontMenuAddUser;
						g_front_inited = 0;
					
				}
				else
				{
					  debugPrint(" *********************************\r\n");
			      debugPrint(" Press 1 :add key user  \r\n");
			      debugPrint(" Press 2 :add fig user  \r\n");
						debugPrint(" Press 3 :add rfid user \r\n");
			      debugPrint(" *********************************\r\n");
				}
			  
        //qDebug() << "FrontAwake";
    }
	
}


//���һ�������û�
static void FrontAddNormalKeyUser(void)
{
	  ParamAwake_t *param = (ParamAwake_t *)&g_front_param;
    u32 id, idx;
    KeyLed_t key;
    if(g_front_inited)
		{
        //��������
				key = FrontGetKey();
        if (key == KEY_LED_NONE)
				{
            param->key_last = key;
        }
				else if (key != param->key_last)
				{
            g_front_stamp = FrontGetStamp();     //����ʱ���
            param->key_last = key;
            VoicePlay(VOICE_BEEP);
            if (key == KEY_LED_HASH)
						{            //#��
                if (param->password_len > 0)
								{  
									 if(param->password_len > 12)
									 {
										  debugPrint(" password more than 12 ,fail\r\n");
									 }
									 else
									 {
										   debugPrint(" \r\n input password : ");
										   g_user_config.password_unlock_hi = -1;
										   g_user_config.password_unlock_lo = -1;
											 for(idx = 0;idx < param->password_len;idx ++)
											 {
													 g_user_config.password_unlock_hi = (g_user_config.password_unlock_hi << 4) | (g_user_config.password_unlock_lo >> 28);
                           g_user_config.password_unlock_lo = (g_user_config.password_unlock_lo << 4) | (param->password_buff[idx] & 0xF);
												   debugPrint(" %d",param->password_buff[idx]);
											 }
											 g_user_config.verify_mode = 0;
											 g_user_config.password_num_len = param->password_len;
											 UserSaveConfig();
											 debugPrint(" \r\n add normal user success  \r\n");
									 }
									 g_front_state = FrontAddNormalUser;
									 g_front_inited = 0;
									 memset(param->password_buff,0,256);
									 param->password_len = 0;
							}
								
            }
					  else if (key == KEY_LED_STAR)
						{   //*��
                //�������
							  if(param->password_len > 0)
								{
									 memset(param->password_buff,0,256);
							     param->password_len = 0;
									 debugPrint("\r\n clear input password   \r\n ");
								}
								else//������һ��
								{ 
									  g_front_state = FrontAddNormalUser;
								    g_front_inited = 0;
								}
            }
						else
						{                               //���ּ�
                //����һλ
							  param->password_buff[param->password_len] = key;
							  param->password_len ++;
							  debugPrint(" %d",key);
            }
        }
				//��ʱ����
        if (FrontCalcElapsed(g_front_stamp) >= FRONT_OVERTIME)
				{
            g_front_state = FrontSleep;
            g_front_inited = 0;
        }
    }
		else
		{
			  memset(param->password_buff,0,256);
				param->password_len = 0;

        g_front_inited = 1;
        g_front_stamp = FrontGetStamp();

			  debugPrint(" *********************************\r\n");
			  debugPrint(" Please add passrord for normal user  \r\n");
			  debugPrint(" *********************************\r\n");
    }
}
static void FrontAddNormalFingerUser(void)
{
		ParamAwake_t *param = (ParamAwake_t *)&g_front_param;
    u32 id, idx;
    if(g_front_inited)
		{
        //��ʱ����
        if (FrontCalcElapsed(g_front_stamp) >= FRONT_OVERTIME)
				{
            g_front_state = FrontAddNormalUser;
            g_front_inited = 0;
        }
    }
		else
		{
			  memset(param->password_buff,0,256);
				param->password_len = 0;

        g_front_inited = 1;
        g_front_stamp = FrontGetStamp();

			  debugPrint(" *********************************\r\n");
			  debugPrint(" Please press finger  \r\n");
			  debugPrint(" *********************************\r\n");
    }
}
static void FrontAddNormalRfidUser(void)
{
	  ParamAwake_t *param = (ParamAwake_t *)&g_front_param;
    u32 id, idx;
    if(g_front_inited)
		{
        //��ʱ����
        if (FrontCalcElapsed(g_front_stamp) >= FRONT_OVERTIME)
				{
            g_front_state = FrontAddNormalUser;
            g_front_inited = 0;
        }
    }
		else
		{
			  memset(param->password_buff,0,256);
				param->password_len = 0;

        g_front_inited = 1;
        g_front_stamp = FrontGetStamp();

			  debugPrint(" *********************************\r\n");
			  debugPrint(" Please add rfid  \r\n");
			  debugPrint(" *********************************\r\n");
    }
	
}
//===========================ǰ��״̬���ӿ�======================================//
//ǰ��״̬����ʼ��
void FrontInit(void)
{
    //��ʼ��״̬Ϊ˯��״̬
    g_front_state = FrontSleep;
    g_front_inited = 0;
    //��ʼ�����ü�״̬Ϊ̧��״̬
    g_config_last = 0;
    //�û������ʼ��
    UserInit();
}
//ǰ��״̬������
void FrontTask(void)
{
    //ǰ��״̬������
    g_front_state();

    //�ж����ü��̰�/����
    u32 config_key = FrontGetConfigKey();
    if ((g_config_last == 0) && (config_key == 1))
		{//���ü�����ʱ��¼ʱ���
        g_config_stamp = FrontGetStamp();
        g_config_triggered = 0;
    }
		else if ((g_config_last == 1) && (config_key == 1))
		{//���������ҳ�ʱʱ����"����"�¼�,ͬʱ��ֹ�����������ֱ��̧�����ü�
        if ((g_config_triggered == 0) && (FrontCalcElapsed(g_config_stamp) >= 3000))
				{
            //qDebug() << "LogPress";
            g_config_triggered = 1;
        }
    }
		else if ((g_config_last == 1) && (config_key == 0))
		{//̧�����ü�ʱ,���û�д���"����"�ʹ���"�̰�"
        if (g_config_triggered == 0)
				{
            //�������Ա��֤״̬
            if ((g_front_state == FrontSleep) || (g_front_state == FrontAwake))
						{
                g_front_state = FrontVerifyAdmin;
                g_front_inited = 0;
            }
        }
        g_config_triggered = 0;
    }
    g_config_last = config_key;
}
