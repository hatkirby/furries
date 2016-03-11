#include <libxml/parser.h>
#include <iostream>
#include <dirent.h>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sqlite3.h>
#include <sstream>
#include <regex>

struct verb {
  std::string infinitive;
  std::string past_tense;
  std::string past_participle;
  std::string ing_form;
  std::string s_form;
};

struct group {
  std::string id;
  std::set<std::string> members;
};

std::map<std::string, group> groups;
std::map<std::string, verb> verbs;
std::map<int, std::map<int, int>> wn;

void print_usage()
{
  std::cout << "Verbly Datafile Generator" << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "Requires exactly four arguments." << std::endl;
  std::cout << "1. The path to a VerbNet data directory." << std::endl;
  std::cout << "2. The path to a SemLink vnpbMappings file." << std::endl;
  std::cout << "3. The path to an AGID infl.txt file." << std::endl;
  std::cout << "4. The path to a WordNet prolog data directory." << std::endl;
  std::cout << "5. Datafile output path." << std::endl;
  
  exit(1);
}
/*
void parse_group(xmlNodePtr top, std::string filename)
{
  xmlChar* key = xmlGetProp(top, (xmlChar*) "ID");
  if (key == 0)
  {
    std::cout << "Bad VerbNet file format: " << filename << std::endl;
    print_usage();
  }
  std::string vnid = key;
  vnid = vnid.substr(vnid.find_first_of("-")+1);
  xmlFree(key);
  
  group g;
  g.id = vnid;
  
  for (xmlNodePtr node = top->xmlChildrenNode; node != nullptr; node = node->next)
  {
    if (!xmlStrcmp(node->name, (const xmlChar*) "MEMBERS"))
    {
      for (xmlNodePtr member = node->xmlChildrenNode; member != nullptr; member = member->next)
      {
        if (!xmlStrcmp(member->name, (const xmlChar*) "MEMBER"))
        {
          key = xmlGetProp(member, (xmlChar*) "name");
          g.members.insert(key);
          xmlFree(key);
        }
      }
    } else if (!xmlStrcmp(node->name, (const xmlChar*) "FRAMES"))
    {
      for (xmlNodePtr frame = node->xmlChildrenNode; frame != nullptr; frame = frame->next)
      {
        if (!xmlStrcmp(frame->name, (const xmlChar*) "FRAME"))
        {
          for (xmlNodePtr framenode = frame->xmlChildrenNode; framenode != nullptr; framenode = framenode->next)
          {
            
          }
        }
      }
    }
  }
}*/

int main(int argc, char** argv)
{
  if (argc != 6)
  {
    print_usage();
  }
  
  /*DIR* dir;
  if ((dir = opendir(argv[1])) == nullptr)
  {
    std::cout << "Invalid VerbNet data directory." << std::endl;
    
    print_usage();
  }
  
  struct dirent* ent;
  while ((ent = readdir(dir)) != nullptr)
  {
    std::string filename(argv[1]);
    if (filename.back() != '/')
    {
      filename += '/';
    }
    
    filename += ent->d_name;
    //std::cout << ent->d_name << std::endl;
    
    if (filename.rfind(".xml") != filename.size() - 4)
    {
      continue;
    }
    
    xmlDocPtr doc = xmlParseFile(filename.c_str());
    if (doc == nullptr)
    {
      std::cout << "Error opening " << filename << std::endl;
      print_usage();
    }
    
    xmlNodePtr top = xmlDocGetRootElement(doc);
    if ((top == nullptr) || (xmlStrcmp(top->name, (xmlChar*) "VNCLASS")))
    {
      std::cout << "Bad VerbNet file format: " << filename << std::endl;
      print_usage();
    }
    
    parse_group(top, filename);
  }
  
  closedir(dir);*/
  
  // Get verbs from AGID
  std::cout << "Reading verb inflection..." << std::endl;
  
  std::ifstream agidfile(argv[3]);
  if (!agidfile.is_open())
  {
    std::cout << "Could not open AGID file: " << argv[3] << std::endl;
    print_usage();
  }
  
  for (;;)
  {
    std::string line;
    if (!getline(agidfile, line))
    {
      break;
    }
    
    if (line.back() == '\r')
    {
      line.pop_back();
    }
    
    int divider = line.find_first_of(" ");
    std::string word = line.substr(0, divider);
    line = line.substr(divider+1);
    
    if (line[0] != 'V')
    {
      continue;
    }
    
    if (line[1] == '?')
    {
      line.erase(0, 4);
    } else {
      line.erase(0, 3);
    }
    
    std::vector<std::string> forms;
    while (!line.empty())
    {
      std::string inflection;
      if ((divider = line.find(" | ")) != std::string::npos)
      {
        inflection = line.substr(0, divider);
        line = line.substr(divider + 3);
      } else {
        inflection = line;
        line = "";
      }
      
      if ((divider = inflection.find_first_of(",?")) != std::string::npos)
      {
        inflection = inflection.substr(0, divider);
      }
      
      forms.push_back(inflection);
    }
    
    verb v;
    v.infinitive = word;
    if (forms.size() == 4)
    {
      v.past_tense = forms[0];
      v.past_participle = forms[1];
      v.ing_form = forms[2];
      v.s_form = forms[3];
    } else if (forms.size() == 3)
    {
      v.past_tense = forms[0];
      v.past_participle = forms[0];
      v.ing_form = forms[1];
      v.s_form = forms[2];
    } else if (forms.size() == 8)
    {
      // As of AGID 2014.08.11, this is only "to be"
      v.past_tense = forms[0];
      v.past_participle = forms[2];
      v.ing_form = forms[3];
      v.s_form = forms[4];
    } else {
      // Words that don't fit the cases above as of AGID 2014.08.11:
      // - may and shall do not conjugate the way we want them to
      // - methinks only has a past tense and is an outlier
      // - wit has five forms, and is archaic/obscure enough that we can ignore it for now
      std::cout << "Ignoring verb \"" << word << "\" due to non-standard number of forms." << std::endl;
    }
    
    verbs[word] = v;
  }
  
  // Start writing output
  std::cout << "Writing output..." << std::endl;
  
  sqlite3* ppdb;
  if (sqlite3_open_v2(argv[5], &ppdb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) != SQLITE_OK)
  {
    std::cout << "Error opening output datafile: " << sqlite3_errmsg(ppdb) << std::endl;
    print_usage();
  }
  
  std::ifstream schemafile("schema.sql");
  if (!schemafile.is_open())
  {
    std::cout << "Could not find schema file" << std::endl;
    print_usage();
  }
  
  std::stringstream schemabuilder;
  for (;;)
  {
    std::string line;
    if (!getline(schemafile, line))
    {
      break;
    }
    
    if (line.back() == '\r')
    {
      line.pop_back();
    }
    
    schemabuilder << line << std::endl;
  }
  
  std::string schema = schemabuilder.str();
  while (!schema.empty())
  {
    std::string query;
    int divider = schema.find(";");
    if (divider != std::string::npos)
    {
      query = schema.substr(0, divider+1);
      schema = schema.substr(divider+2);
    } else {
      break;
    }
    
    sqlite3_stmt* schmstmt;
    if (sqlite3_prepare_v2(ppdb, query.c_str(), query.length(), &schmstmt, NULL) != SQLITE_OK)
    {
      std::cout << "Error writing to output database: " << sqlite3_errmsg(ppdb) << std::endl;
      sqlite3_close_v2(ppdb);
      print_usage();
    }
  
    if (sqlite3_step(schmstmt) != SQLITE_DONE)
    {
      std::cout << "Error writing to output database: " << sqlite3_errmsg(ppdb) << std::endl;
      sqlite3_close_v2(ppdb);
      print_usage();
    }
  
    sqlite3_finalize(schmstmt);
  }
  
  std::cout << "Writing verbs..." << std::endl;
  for (auto& mapping : verbs)
  {
    sqlite3_stmt* ppstmt;
    std::string query("INSERT INTO verbs (infinitive, past_tense, past_participle, ing_form, s_form) VALUES (?, ?, ?, ?, ?)");
    if (sqlite3_prepare_v2(ppdb, query.c_str(), query.length(), &ppstmt, NULL) != SQLITE_OK)
    {
      std::cout << "Error writing to output database: " << sqlite3_errmsg(ppdb) << std::endl;
      sqlite3_close_v2(ppdb);
      print_usage();
    }
    
    sqlite3_bind_text(ppstmt, 1, mapping.second.infinitive.c_str(), mapping.second.infinitive.length(), SQLITE_STATIC);
    sqlite3_bind_text(ppstmt, 2, mapping.second.past_tense.c_str(), mapping.second.past_tense.length(), SQLITE_STATIC);
    sqlite3_bind_text(ppstmt, 3, mapping.second.past_participle.c_str(), mapping.second.past_participle.length(), SQLITE_STATIC);
    sqlite3_bind_text(ppstmt, 4, mapping.second.ing_form.c_str(), mapping.second.ing_form.length(), SQLITE_STATIC);
    sqlite3_bind_text(ppstmt, 5, mapping.second.s_form.c_str(), mapping.second.s_form.length(), SQLITE_STATIC);
    
    if (sqlite3_step(ppstmt) != SQLITE_DONE)
    {
      std::cout << "Error writing to output database: " << sqlite3_errmsg(ppdb) << std::endl;
      sqlite3_close_v2(ppdb);
      print_usage();
    }
    
    sqlite3_finalize(ppstmt);
  }
  
  // Get nouns/adjectives/adverbs from WordNet
  // Useful relations:
  // - s: master list
  // - ant: antonymy (e.g. happy/sad, sad/happy, happiness/sadness)
  // - at: variation (e.g. a measurement can be standard or nonstandard)
  // - hyp: hypernymy/hyponymy (e.g. color/red, color/blue)
  // - ins: instantiation (do we need this? let's see)
  // - mm: member meronymy/holonymy (e.g. family/mother, family/child)
  // - mp: part meronymy/holonymy (e.g. wheel/spoke, wheel/tire)
  // - ms: substance meronymy/holonymy (e.g. tire/rubber, doorstop/rubber)
  // - per: pertainymy (e.g. something that is Alaskan pertains to Alaska)
  //        mannernymy (e.g. something done quickly is done in a manner that is quick)
  // - sa: specification (e.g. inaccurate (general) can mean imprecise or incorrect (specific))
  // - sim: synonymy (e.g. cheerful/happy, happy/cheerful)
  // - syntax: positioning flags for some adjectives
  std::string wnpref {argv[4]};
  if (wnpref.back() != '/')
  {
    wnpref += '/';
  }
  
  std::cout << "Reading words from WordNet..." << std::endl;
  std::ifstream wnsfile(wnpref + "wn_s.pl");
  if (!wnsfile.is_open())
  {
    std::cout << "Invalid WordNet data directory." << std::endl;
    print_usage();
  }
  
  for (;;)
  {
    std::string line;
    if (!getline(wnsfile, line))
    {
      break;
    }
    
    if (line.back() == '\r')
    {
      line.pop_back();
    }
    
    std::regex relation("^s\\(([134]\\d{8}),(\\d+),'([\\w ]+)',");
    std::smatch relation_data;
    if (!std::regex_search(line, relation_data, relation))
    {
      continue;
    }
    
    int synset_id = stoi(relation_data[1]);
    int wnum = stoi(relation_data[2]);
    std::string word = relation_data[3];
    
    std::string query;
    switch (synset_id / 100000000)
    {
      case 1: // Noun
      {
        query = "INSERT INTO nouns (form) VALUES (?)";
        
        break;
      }
      
      case 2: // Verb
      {
        // Ignore
        
        break;
      }
      
      case 3: // Adjective
      {
        query = "INSERT INTO adjectives (form) VALUES (?)";
        
        break;
      }
      
      case 4: // Adverb
      {
        query = "INSERT INTO adverbs (form) VALUES (?)";
        
        break;
      }
    }
    
    sqlite3_stmt* ppstmt;
    if (sqlite3_prepare_v2(ppdb, query.c_str(), query.length(), &ppstmt, NULL) != SQLITE_OK)
    {
      std::cout << "Error writing to output database: " << sqlite3_errmsg(ppdb) << std::endl;
      sqlite3_close_v2(ppdb);
      print_usage();
    }
    
    sqlite3_bind_text(ppstmt, 1, word.c_str(), word.length(), SQLITE_STATIC);
    
    if (sqlite3_step(ppstmt) != SQLITE_DONE)
    {
      std::cout << "Error writing to output database: " << sqlite3_errmsg(ppdb) << std::endl;
      sqlite3_close_v2(ppdb);
      print_usage();
    }
    
    sqlite3_finalize(ppstmt);
    
    query = "SELECT last_insert_rowid()";
    if (sqlite3_prepare_v2(ppdb, query.c_str(), query.length(), &ppstmt, NULL) != SQLITE_OK)
    {
      std::cout << "Error writing to output database: " << sqlite3_errmsg(ppdb) << std::endl;
      sqlite3_close_v2(ppdb);
      print_usage();
    }
    
    if (sqlite3_step(ppstmt) != SQLITE_ROW)
    {
      std::cout << "Error writing to output database: " << sqlite3_errmsg(ppdb) << std::endl;
      sqlite3_close_v2(ppdb);
      print_usage();
    }
    
    wn[synset_id][wnum] = sqlite3_column_int(ppstmt, 0);
    
    sqlite3_finalize(ppstmt);
  }
  
  sqlite3_close_v2(ppdb);
  
  std::cout << "Done." << std::endl;
}