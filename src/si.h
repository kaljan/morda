#ifndef SI_H
#define SI_H

int show_cpu_temp(void);
int get_time(char *str);
int show_date(void);
int show_week_day(void);

int get_ip_addr(const char *iface, char *ipstr);


#endif // SI_H
