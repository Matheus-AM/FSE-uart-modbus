#ifndef PID_H_
#define PID_H_

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

void pid_configura_constantes(double Kp_, double Ki_, double Kd_);
void pid_atualiza_referencia(float referencia_);
double pid_controle(double saida_medida);


#ifdef __cplusplus
}
#endif /*! CPP guard */

#endif /* PID_H_ */