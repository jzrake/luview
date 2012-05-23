


#ifdef __cplusplus
extern "C" {
#endif

#ifndef __pyplotcm_HEADER__
#define __pyplotcm_HEADER__

int pyplot_colors_get_num_tables();
const char **pyplot_colors_get_names();
const float *pyplot_colors_get_lookup_table(const char *key);
const float *pyplot_colors_get_lookup_table_by_id(int id);

#endif // __pyplotcm_HEADER__

#ifdef __cplusplus
}
#endif
