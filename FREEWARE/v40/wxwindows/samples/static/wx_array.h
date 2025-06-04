

#ifndef wx_arrayh

#define _wx_arrayh



#define WX_ARRAY(NAME, CAST) \
 	  class NAME##Node \
 	     { \
 	            char *desc_; \
 	            CAST *data_; \
 	            long count_; \
 	       public : \
 	            NAME##Node(char *desc,CAST *data) \
 	                    { \
 	                      count_ = 1; data_ = data; \
 	                      desc_ = new char[strlen(desc) + 1]; \
 	                      strcpy(desc_,desc); \
 	                    }; \
 	            ~NAME##Node(void) { delete (char*) desc_; \
 	                              delete (CAST *) data_; \
 	                            }; \
 	            long getCount(void) const { return count_; }; \
                CAST *getData(void) const { return data_; }; \
                char *getDesc(void) const { return desc_; }; \
                void incCount(void) { count_++; }; \
                void decCount(void) { \
                                      if (count_ > 0) count_--; \
                                      else cout << "\nERROR ! The node already empty !\n"; \
                                    };  \
 	     }; \
 	     \
    class NAME##Array \
 	     { \
 	       NAME##Node **array_; \
 	       long length_; \
 	       public : \
 	            NAME##Array()  { \
 	               array_ = NULL; \
 	               length_ = 0; \
 	               cout << "\n List !! \n"; }; \
 	            ~NAME##Array() { \
 	                cout << "\n Deleting \n"; deleteArray(); }; \
                long add(NAME##Node *node) \
                   { \
                     long llp; \
                     for(llp = 0;llp < length_;llp++) \
                          if (array_[llp] == NULL) \
                             { \
                               array_[llp] = (NAME##Node *) node; \
                               return llp; \
                             } \
                     NAME##Node **narr = new NAME##Node *[length_ + 1]; \
                     for(llp = 0;llp < length_;llp++) \
                          narr[llp] = array_[llp]; \
                     delete (NAME##Node **) array_; \
                     array_ = narr; \
                     array_[length_] = (NAME##Node *) node; \
                     length_++; \
                     return length_ - 1; \
                   }; \
                long add(char *desc, CAST * data) \
                   { \
                     NAME##Node *node = new NAME##Node(desc,(CAST *) data); \
                     return add((NAME##Node *) node); \
                   }; \
                long getLength(void) const { return length_; }; \
                long find_by_data(CAST * data) \
                                       {  \
                                         long llp; \
                                         for(llp = 0;llp < length_;llp++) \
                                              if (array_[llp] && array_[llp] -> getData() == data) return llp; \
                                         return -1; \
                                       }; \
                long find_by_desc(char *desc) \
                                       {  \
                                         long llp; \
                                         for(llp = 0;llp < length_;llp++) \
                                              if (array_[llp] && strcmp(array_[llp] -> getDesc(),desc) == 0) return llp; \
                                         return -1; \
                                       }; \
                Int deleteNode(long index) \
                               { \
                                 cout <<"\n\n Deleting ! length = " << length_ << "\n\n";\
                                 if ((index < 0) || (index >= length_)) \
                                    { \
                                      cout << "\nERROR : Invalid index range !" << length_ << " index " << index << "\n"; \
                                      return -1; \
                                    }; \
                                 delete (NAME##Node *) array_[index]; \
                                 array_[index] = NULL; \
                                 return 0; \
                               }; \
                void trashArray(void) \
                                { \
                                  long llp; \
                                  for(llp = 0;llp < length_;llp++) \
                                       if (array_[llp] -> getCount() == 0) \
                                            deleteNode(llp); \
                                }; \
                void deleteArray(void) \
                                 { \
                                   long llp; \
                                   for(llp = 0;llp < length_;llp++) \
                                      { \
                                        if (array_[llp] == NULL) continue; \
                                        if (array_[llp] -> getCount() != 0) \
                                             cout << "\nWARNING ! Node " << llp << " not empty !\n"; \
                                        deleteNode(llp); \
                                      } \
                                   delete (NAME##Node **) array_; \
                                 }; \
                long find_or_create(char *desc) \
                                     { \
                                       cout <<"\n Adding ! length = " << length_ << "\n";\
                                       long llp = find_by_desc(desc); \
                                       if (llp == -1) \
                                          { \
                                            CAST *data = new CAST (desc); \
                                            llp = add(desc,data); \
                                            return llp; \
                                          } \
                                       else \
                                          { \
                                            incCount(llp); \
                                            return llp; \
                                          } \
                                     }; \
                int incCount(long index) \
                              { \
                                if ((index < 0) || (index >= length_)) \
                                   { \
                                     cout << "\nERROR : 2 Invalid index range !" << length_ << " index " << index << "\n"; \
                                     return -1; \
                                   }; \
                                array_[index] -> incCount(); \
                              }; \
                int decCount(long index) \
                              { \
                                if ((index < 0) || (index >= length_)) \
                                   { \
                                     cout << "\nERROR : 3 Invalid index range !" << length_ << " index " << index << "\n"; \
                                     return -1; \
                                   }; \
                                if (array_[index] != NULL)  \
                                   { \
                                     array_[index] -> decCount(); \
                                     if (array_[index] -> getCount() == 0) \
                                          deleteNode(index); \
                                   } \
                                else cout << "\n\nError the node is corrupt, Lohool \n\n\n"; \
                              }; \
 	            NAME##Node **getArray(void) const { return array_; }; \
 	     };

#endif
