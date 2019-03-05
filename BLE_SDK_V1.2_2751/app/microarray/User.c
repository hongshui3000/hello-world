#include "User.h"
#include "Front.h"
#include <string.h>
#include "m_flash.h"
typedef struct{
    u32 password_lo;                      //���������8λ
    u32 password_hi;                      //���������4λ
	  u32 password_num_len;
    VerifyMode_t verify_mode;                    //����ģʽ
}UserData_t;                                     //���浽Flash���û�������������

#define FLASH_ERASE_SIZE     4096                //Flash����ҳ��С
#define MAX_USER_DATA_NUM    (FLASH_ERASE_SIZE / sizeof(UserData_t))//һ������ҳ�п��Ա�����û����ݸ���

//===========================Flash����ӿں���==================================//
//Flash��Ϊ3ҳ,�ֱ𱣴��û��������ݣ�RFID��ָ��
//extern ParamAwake_t g_front_param;//����״̬�Ĳ����ṹ;

//Flash��ʼ��
void FlashInit(void)
{
  
}

//ADMIN_BASEADDR


//��Flash�ж���һ������Ա�û����ݽṹ
void FlashReadAdminData(u32 idx, UserData_t *p_data)
{
		flash_read(ADMIN_BASEADDR + idx * sizeof(UserData_t), sizeof(UserData_t), (uint8_t * )p_data);
	/*
    if (g_flash.isOpen()){
        g_flash.seek(idx * sizeof(UserData_t));
        g_flash.read((char *)p_data, sizeof(UserData_t));
    }*/
}
//��Flash��д��һ������Ա�û����ݽṹ
void FlashWriteAdminData(u32 idx, UserData_t *p_data)
{
	 eflash_write(ADMIN_BASEADDR + idx * sizeof(UserData_t), sizeof(UserData_t), (uint32_t *)p_data);
	/*
    if (g_flash.isOpen()){
        g_flash.seek(idx * sizeof(UserData_t));
        g_flash.write((char *)p_data, sizeof(UserData_t));
    }*/
}
//��������Ա�û���������ҳ
void FlashEraseAdminData(void)
{
	 eflash_page_erase(ADMIN_BASEADDR);
	/*
    if (g_flash.isOpen()){
        u32 fill = -1;
        for(u32 i = 0; i < FLASH_ERASE_SIZE / sizeof(u32); i++){
            g_flash.write((const char *)&fill, sizeof(u32));
        }
    }*/
}



//��Flash�ж���һ���û����ݽṹ
void FlashReadUserData(u32 idx, UserData_t *p_data)
{
		flash_read(KEY_MAIN_BASEADDR + idx * sizeof(UserData_t), sizeof(UserData_t), (uint8_t * )p_data);
	/*
    if (g_flash.isOpen()){
        g_flash.seek(idx * sizeof(UserData_t));
        g_flash.read((char *)p_data, sizeof(UserData_t));
    }*/
}
//��Flash��д��һ���û����ݽṹ
void FlashWriteUserData(u32 idx, UserData_t *p_data)
{
	 eflash_write(KEY_MAIN_BASEADDR + idx * sizeof(UserData_t), sizeof(UserData_t), (uint32_t *)p_data);
	/*
    if (g_flash.isOpen()){
        g_flash.seek(idx * sizeof(UserData_t));
        g_flash.write((char *)p_data, sizeof(UserData_t));
    }*/
}
//�����û���������ҳ
void FlashEraseUserData(void)
{
	 eflash_page_erase(KEY_MAIN_BASEADDR);
	/*
    if (g_flash.isOpen()){
        u32 fill = -1;
        for(u32 i = 0; i < FLASH_ERASE_SIZE / sizeof(u32); i++){
            g_flash.write((const char *)&fill, sizeof(u32));
        }
    }*/
}

//��Flash��ȡ��һ��RFID
u32 FlashReadRfid(u32 idx)
{
    u32 rfid = -1;
	  flash_read(RFID_MAIN_BASEADDR + idx * sizeof(u32), sizeof(u32), (uint8_t * )&rfid);
/*
    if (g_flash.isOpen()){
        g_flash.seek(idx * sizeof(u32) + FLASH_ERASE_SIZE);
        g_flash.read((char *)&rfid, sizeof(u32));
    }*/
    return rfid;
}
//��Flash��д��һ��RFID
void FlashWriteRfid(u32 idx, u32 rfid)
{
		eflash_program(RFID_MAIN_BASEADDR + idx * sizeof(u32), rfid);
	/*
    if (g_flash.isOpen()){
        g_flash.seek(idx * sizeof(u32) + FLASH_ERASE_SIZE);
        g_flash.write((char *)&rfid, sizeof(u32));
    }*/
}
//����RFID��������ҳ
void FlashEraseRFID(void)
{
	 eflash_page_erase(RFID_MAIN_BASEADDR);
		/*
    if (g_flash.isOpen()){
        g_flash.seek(FLASH_ERASE_SIZE);
        u32 fill = -1;
        for(u32 i = 0; i < FLASH_ERASE_SIZE / sizeof(u32); i++){
            g_flash.write((const char *)&fill, sizeof(u32));
        }
    }*/
}

//===========================�û�����ӿ�=======================================//
volatile UserConfig_t g_user_config;                      //ȫ���û�����

//�û������ʼ��
void UserInit(void)
{
    u32 index,RFID;
    volatile UserData_t user_data;
	
    //TODO:��Flash�����Ƶ�hardwareĿ¼����ȥ�����޸Ľӿ�
    FlashInit();
    g_user_config.admin_write_index = 0;
	  //�������µĹ���Ա�û�����
		for (index = 0; index < MAX_USER_DATA_NUM; index++){
					//�������һ�����벻ΪȫF�Ľṹ,�������µ�����
					FlashReadAdminData(index, (UserData_t *)&user_data);
					if (user_data.password_num_len == -1)
					{
							break;
					}
			}

		g_user_config.admin_write_index = index;

		if (index > 0){
        //�������µ�����
        FlashReadAdminData(index - 1, (UserData_t *)&user_data);
        g_user_config.password_admin_lo = user_data.password_lo;
        g_user_config.password_admin_hi = user_data.password_hi;
        g_user_config.verify_mode = user_data.verify_mode;
				g_user_config.password_num_len = user_data.password_num_len;
    }else{
        //û�����µ����þ�ʹ��Ĭ������
        g_user_config.password_admin_lo = 0xFF123456;
        g_user_config.password_admin_hi = 0xFFFFFFFF;
			  g_user_config.password_num_len = 6;
        g_user_config.verify_mode = VERIFY_MODE_ADMIN_USER;
    }
		
		
    //�������µ��û�����
    for (index = 0; index < MAX_USER_DATA_NUM; index++){
        //�������һ�����벻ΪȫF�Ľṹ,�������µ�����
        FlashReadUserData(index, (UserData_t *)&user_data);
        if (user_data.password_num_len == -1)
				{
            break;
				}
    }
    g_user_config.config_write_index = index;
#if 0
    if (index > 0){
        //�������µ�����
        FlashReadUserData(index - 1, (UserData_t *)&user_data);
        g_user_config.password_unlock_lo = user_data.password_lo;
        g_user_config.password_unlock_hi = user_data.password_hi;
        g_user_config.verify_mode = user_data.verify_mode;
    }else{
        //û�����µ����þ�ʹ��Ĭ������
        g_user_config.password_admin_lo = 0xFF123456;
        g_user_config.password_admin_hi = 0xFFFFFFFF;
        g_user_config.password_unlock_lo = 0xFF123456;
        g_user_config.password_unlock_hi = 0xFFFFFFFF;
			  g_user_config.password_num_len = 6;
        g_user_config.verify_mode = VERIFY_MODE_ADMIN_USER;
			 
			  ChangeNumtoString();
			 // CheckKey(testbuf,100,(u8 *)g_user_config.passwordnum,6);
			
			  user_data.password_lo = 0xFF123456;
        user_data.password_hi = 0xFFFFFFFF;
			  user_data.password_num_len = 6;
			  user_data.verify_mode = VERIFY_MODE_NORMAL_USER;
    }
#endif

    //�������е�RFID��
    for(index = 0; index < MAX_RFID_NUM; index++){
        if (FlashReadRfid(index) == -1){
            break;
        }
    }
		g_user_config.rfid_write_index = index;

#if 0	
		//for test  flash
		for (index = 0; index < MAX_USER_DATA_NUM; index++){
        //�������һ�����벻ΪȫF�Ľṹ,�������µ�����
        FlashWriteUserData(index, &user_data);
    }
		
		RFID = 0x55aaa55a;
		for(index = 0; index < 5; index++)
		{
        FlashWriteRfid(index,RFID);

    }
		
		//FlashEraseUserData();
		//FlashEraseRFID();
 #endif
}


//�����û�����
void AdminSaveConfig(void)
{
    UserData_t user_data;

    //����û������Ѿ�����,���Ȳ�����ҳ
    if (g_user_config.admin_write_index >= MAX_USER_DATA_NUM)
		{
        FlashEraseAdminData();
        g_user_config.admin_write_index = 0;
    }
    //д���û����õ�Flash
   
    
    user_data.verify_mode = g_user_config.verify_mode;
		if(user_data.verify_mode)//����Ա
		{
				 user_data.password_lo = g_user_config.password_admin_lo;
         user_data.password_hi = g_user_config.password_admin_hi;
		}
		else//error
		{
				debugPrint(" \r\n add admin verify mode error  \r\n");
		}
		user_data.password_num_len = g_user_config.password_num_len;
    FlashWriteAdminData(g_user_config.admin_write_index++, &user_data);
}
//�����û�����
void UserSaveConfig(void)
{
    UserData_t user_data;

    //����û������Ѿ�����,���Ȳ�����ҳ
    if (g_user_config.config_write_index >= MAX_USER_DATA_NUM)
		{
        FlashEraseUserData();
        g_user_config.config_write_index = 0;
    }
    //д���û����õ�Flash
   
    
    user_data.verify_mode = g_user_config.verify_mode;
		if(user_data.verify_mode)//error
		{
				  debugPrint(" \r\n add user verify mode error  \r\n");
		}
		else//��ͨ�û�
		{
				user_data.password_lo = g_user_config.password_unlock_lo;
				user_data.password_hi = g_user_config.password_unlock_hi;
		}
		user_data.password_num_len = g_user_config.password_num_len;
    FlashWriteUserData(g_user_config.config_write_index++, &user_data);
}
//���ҿ���,���ش��ڵ���0������,����-1��ʾδ�ҵ�
s32  UserFindRfid(u32 rfid)
{
    u32 index;
    u32 tmp_id;
    for(index = 0; index < MAX_RFID_NUM; index++){
        tmp_id = FlashReadRfid(index);
			  if(tmp_id == -1){
					 return -1;
				}
        if (tmp_id == rfid){
            return index;
        }
    }
    return -1;
}
//���һ������
s32 UserAddRfid(u32 rfid)
{
    if(g_user_config.rfid_write_index >= MAX_RFID_NUM){
        return -1;
    }
    FlashWriteRfid(g_user_config.rfid_write_index++, rfid);
    return 0;
}
//ɾ�����п���
void UserDeleteAllRfid(void)
{
    FlashEraseRFID();
    g_user_config.rfid_write_index = 0;
}
void UserDeleteAllKeyUser(void)
{
	 FlashEraseUserData();
   g_user_config.config_write_index = 0;
	
}
void UserDeleteAllAdminUser(void)
{
	 FlashEraseAdminData();
   g_user_config.admin_write_index = 0;
}


u8  UserFindAdmin(void)
{
	u32 index;
	uint8_t findresult;
	UserData_t user_data;
	//ChangeNumtoString();
	for (index = 0; index < MAX_USER_DATA_NUM; index++)
	{
			//�������һ�����벻ΪȫF�Ľṹ,�������µ�����
			FlashReadAdminData(index, (UserData_t *)&user_data);
			if (user_data.password_num_len == -1)
			{
					return 0;
			}
			g_user_config.password_unlock_hi = user_data.password_hi;
      g_user_config.password_unlock_lo = user_data.password_lo;	
			g_user_config.password_num_len = user_data.password_num_len;
      ChangeNumtoString();			
			findresult = CheckKey(g_front_param.password_buff,g_front_param.password_len,(u8 *)g_user_config.passwordnum,g_user_config.password_num_len);
			if(findresult)
			{
				  return 1;
			}
	}
	return 0;
}

u8 UserFindKeyUser(void)
{
	u32 index;
	uint8_t findresult;
	UserData_t user_data;
	//�������µ��û�����
	for (index = 0; index < MAX_USER_DATA_NUM; index++){
			//�������һ�����벻ΪȫF�Ľṹ,�������µ�����
			FlashReadUserData(index, (UserData_t *)&user_data);
			if (user_data.password_num_len == -1)
			{
					return 0;
			}
			g_user_config.password_unlock_hi = user_data.password_hi;
      g_user_config.password_unlock_lo = user_data.password_lo;	
			g_user_config.password_num_len = user_data.password_num_len;			
      ChangeNumtoString();			
			findresult = CheckKey(g_front_param.password_buff,g_front_param.password_len,(u8 *)g_user_config.passwordnum,g_user_config.password_num_len);
			if(findresult)
			{
				  return 1;
			}
			
	}
	
	
}

