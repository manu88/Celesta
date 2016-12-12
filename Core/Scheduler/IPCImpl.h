//
//  IPC.h
//  testIPC2
//
//  Created by Manuel Deneu on 29/06/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __IPC__IMPL__
#define __IPC__IMPL__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>


#include <string.h>


/*
 Message system
 */

enum { max_msg_size = 128 };

struct msgBuffer
{
    long mtype;  /* must be positive */
    struct info
    {
        char name[ max_msg_size ];
    } info;
};


namespace IPC_IMPL
{

class Base
{
public:
    
    bool connected() const
    {
        return _msqid != -1;
    }
    
protected:
    Base():
    _key   ( -1 ),
    _msqid ( -1 )
    {}
    
    ~Base()
    {
        if( connected() )
            close();
    }
    
    bool connect(const std::string &file )
    {
        if( createKeyFromFile( file ))
            return create();
        
        return false;
    }
    
    bool connect(key_t key )
    {
        createFromKey( key );

        return create();
    }
    
    void createFromKey( key_t key)
    {
        _key = key;
    }
    
    bool createKeyFromFile( const std::string &file)
    {
        _key = ftok(file.c_str(), 'B');
        
        return _key != -1;
    }
    
    bool create()
    {
        _msqid = msgget( _key, 0644 | IPC_CREAT);
        
        return _msqid != -1;
    }
    
    bool connect()
    {
        _msqid = msgget( _key, 0644 );
        
        
        return _msqid != -1;
    }
    
    bool close()
    {
        return msgctl( _msqid, IPC_RMID, NULL) != -1;
    }
    
    key_t _key;
    int   _msqid;
};

class Sender : public Base
{
public:
    
    Sender()
    {

    }
    ~Sender()
    {
        //close();
    }
    bool sendMessage( const std::string &msg , int type)
    {
        if( !connected() )
            return false;
        struct msgBuffer pmb = {type, { "default"} };
        
        strcpy(pmb.info.name, msg.c_str() );
        

        return msgsnd( _msqid, &pmb, sizeof(struct msgBuffer) - sizeof(long), 0) == 0;
    }
    
};

class Listener : public Base
{
public:
    
    Listener() :
    _typeToRead ( 1 )
    {

    }
    ~Listener()
    {
        close();
    }
    
    void setTypeToRead( long type)
    {
        _typeToRead = type;
    }
    
    long getTypeToRead() const
    {
        return _typeToRead;
    }
    
    bool readMsg( struct msgBuffer *msg)
    {
        if( connected() )
        {
            const long r = msgrcv( _msqid, msg, sizeof(struct msgBuffer) - sizeof(long), _typeToRead, IPC_NOWAIT );
            
            if( r > 1 )
                return true;
        }
        
        return false;
    }
    
private:
    long _typeToRead;
};
    

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*
 Shared Memory
 */
struct ObjTest
{
    int a;
    float b;
    std::string str;
    
};

class SharedBase
{
public:
    
    
    SharedBase():
    _key       ( -1),
    _shmid     ( -1),
    _allocated ( 0 ),
    _data(nullptr)
    {}
    
    ~SharedBase()
    {}
    
    void createFromKey( key_t key)
    {
        _key = key;
    }
    
    bool createKeyFromFile( const std::string &file)
    {
        _key = ftok(file.c_str(), 'R');
        
        return _key != -1;
    }
    
    bool connect(size_t size)
    {
        if (( _shmid = shmget( _key, size , 0644 | IPC_CREAT)) == -1)
        {
            perror("shmget");
            return false;
        }
        _allocated = size;
        return true;
    }
    
    bool allocate(size_t size )
    {
        if( !connect( size ))
            return false;
        
        _data = shmat(_shmid, (void *)0, 0);
        
        if (_data == NULL)
        {
            perror("shmat");
            return false;
        }
        _allocated = size;
        
        return true;
    }
    
    bool write( const struct ObjTest &obj  )
    {
        
        
        /*
         struct ObjTest obj;
         obj.a = 42;
         obj.b = 124.124;
         
         obj.str = "hello from inside";
         */
        memcpy( _data, &obj, _allocated );
        return true;
        
    }
    
    void* read()
    {
        
        _data =(char*) shmat(_shmid, (void *)0, 0);
        if (_data == (void*) -1 )
        {
            perror("shmat");
            return nullptr;
        }
        
        return _data;
    }
    
    bool disconnect()
    {
        if( !_data)
            return false;
        
        if (shmdt(_data) == -1)
        {
            perror("shmdt");
            return false;
        }
        
        return true;
    }
    
private:
    key_t  _key;
    int    _shmid;
    
    size_t _allocated;
    void  *_data;
    
};
    


} // namespace IPC_IMPL

#endif /* defined(__IPC__IMPL__) */
