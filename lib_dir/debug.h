
#define DEBUG // à mettre en commentaire ou non au besoin

#ifdef DEBUG

# define DEBUG_PRINT(message) debug(message) // ou par RS-232

#else

# define DEBUG_PRINT(message) do {} while (0) // code mort

#endif



