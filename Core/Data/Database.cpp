//
//  Database.cpp
//  
//
//  Created by Manuel Deneu on 06/09/14.
//
//

#include "Database.h"

Database::~Database()
{

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool Database::parseFile(const std::string &fileName , const char delim)
{
    if (!FileSystem::fileExists( fileName))
        return false;
    
    FILE* file = fopen(fileName.c_str() , "r");
    
    if (!file)
        return false;
    
    char line[256];
    
    int count = 0;
    while (fgets(line, sizeof(line), file))
    {

        std::string item = "";
        std::string val = "";
        

        if ( strncmp(line, "#", 1 ) != 0 ) // skipped if begin with '#' ( commentary )
        {
            // remove # comms at end of line
            if( strstr(line, "#") != NULL)
                strtok(line,"#");


            std::stringstream ss(line);
            
            std::getline(ss, item, delim);
            std::getline(ss, val, delim);
            
            if ( !StringOperations::isEmpty( item) && !StringOperations::isEmpty( val )) // acceptable pair item/value, ie. not empty
            {
                bool overwrite = true;
                
                
                if ( item.find("+") != std::string::npos )
                {
                    
                    
                    item.erase(item.end() -1);
                    
                    overwrite = false;
                }
                if (delim != ' ' )
                {
                    item.erase( remove_if(  item.begin(), item.end(), ::isspace ), item.end());
                    
                    /* trim value, ie remove leading & trailing spaces */
                    val = StringOperations::trim( val , " \t\n" );
                    
                    if (!StringOperations::contains(val, "\""))
                    {
                    }
                    
                    else // quoted token
                    {
                        // si un seul " trouvé et dans la premiere moité de la chaine, considérée comme premier,
                        // sinon considéré comme second
                        const std::string::size_type halfSize = val.size() / 2;
                        
                        std::string::size_type first  = 0;
                        
                        first = val.find( "\"", first );
                        
                        std::string::size_type second = first + 1 ;
                        
                        second = val.find( "\"", second );
                        
                        if ( second == std::string::npos)
                        {
                            const std::string::size_type pos = first;
                            if ( pos < halfSize)
                            {
                                val.append("\"");
                                second = val.size() -1;
                            }
                            else
                            {
                                second = pos;
                                first = 0;
                                val.insert(0, "\"");
                            }
                            
                        }
                        
                        val.erase( val.begin() + static_cast<long>( second)  , val.end() );
                        val.erase( val.begin()           , val.begin() +  static_cast<long>(first +1) );
                        
                    }
                }
                
                if (val != "" ) // valid ( &nice ) pair
                {
                    if ( itemExists( item )) // existant
                    {
                        
                        
                        auto pos = findItemPosition( item );
                        std::string oldVal = getValueForItemName(item)->getString();
                        
                        if ( pos != _dataList.end() )
                        {
                            _dataList.erase( pos);
                            count--;
                            
                            if (!overwrite) // on écrase la valeur
                            {
                                
                                
                                val += " " + oldVal;
                                
                            }
                        }
                        
                    }
                    
                    count++;
                    insertValue ( item , Variant( val ) );
                }
            }
            
        }
        
    }
    
    if ( count != (int) _dataList.size() )
    {
        /*list size and iter count don't match */
        DEBUG_ASSERT( false );
        
    }
    
    fclose(file);
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool Database::saveToFile(const std::string &fileName , const char delim  ) const
{
    
    std::ofstream file;
    
    file.open( fileName.c_str() );
    
    if ( !file.is_open() )
        return false;
    
    /*
    Date now;
    Timecode t;
    t.setToCurrentTime();
    
    file << "########## Database file generated on " << now.toString().c_str()
    << " at " << t.getString(true,true,true,false).c_str() << " ##########" ;
    
    
    file <<  "\n";
    file <<  "\n";
    */
    for (const DataPair &pair : _dataList )
    {
        file << pair.first << " " << delim << " " << pair.second.getString();
        file <<  "\n";
        file <<  "\n";
        
    }
    
    
    
    file.close();
    return true;
}



const VariantList Database::getValueForItemNameAsVector( const std::string &item) const
{
    VariantList list;
    
    std::istringstream f( getValueForItemName( item )->getString() );
    std::string s;
    
    while (getline(f, s, ' '))
    {
        if ( !s.empty() )
            list.push_back( Variant( s ) );
    }
    
    return list;
    
}









