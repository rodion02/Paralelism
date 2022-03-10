#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define n 128

int main() {
	    double **u = (double **) calloc(n, sizeof(double *)); // first
	        double **up = (double **) calloc(n, sizeof(double *));  // second
		    for (int i = 0; i < n; i++) {
			            u[i] = (double *) calloc(n, sizeof(double));
				            up[i] = (double *) calloc(n, sizeof(double));
					        }

		        double x1 = 10.0;
			    double x2 = 20.0;
			        double y1 = 30.0;
				    double y2 = 20.0;
				        u[0][0] = up[0][0] = x1;
					    u[0][n-1] = up[0][n-1] = x2;
					        u[n-1][0] = up[n-1][0] = y1;
						    u[n-1][n-1] = up[n-1][n-1] = y2;

						        double step1 = (x2 - x1)/(n-1);
							    double step2 = (y2 - y1)/(n-1);
							        double step3 = (y1 - x1)/(n-1);
#pragma acc kernels
								    for (int i = 1; i<n-1; i++){
									            u[0][i] = x1 + i*step1;
										            u[n-1][i] = y1 + i*step2;
											            u[i][0] = x1 + i*step3;
												            u[i][n-1] = 20.0;
													        }

								        int itter = 0;
									    double error = 1.0;

#pragma acc data copy(u[0:n][0:n], error) create (up[0:n][0:n])
									        {
											        while (itter < 1000000 && error > 1e-6) {
													            if (itter % 100 == 0) {
#pragma acc kernels async
															                    error = 0.0;
																	                }
														                itter++;
#pragma acc data present(u, up)
#pragma acc parallel num_gangs(128) async
																            {
																		                    if (itter % 100 == 0) {
#pragma acc loop collapse(2) independent reduction(max:error)
																					                        for (int i = 1; i < n - 1; i++) {
																									                        for (int j = 1; j < n - 1; j++) {
																													                            up[i][j] = 0.25 * (u[i][j - 1] + u[i][j + 1] + u[i + 1][j] + u[i - 1][j]);
																																                                error = fmax(error, up[i][j] - u[i][j]);
																																				                        }
																												                    }
																								                } else {
#pragma acc loop collapse(2)
																											                    for (int i = 1; i < n - 1; i++) {
																														                            for (int j = 1; j < n - 1; j++) {
																																		                                up[i][j] = 0.25 * (u[i][j - 1] + u[i][j + 1] + u[i + 1][j] + u[i - 1][j]);
																																						                        }
																																	                        }
																													                    }
																				                }
#pragma acc parallel loop independent collapse(2) async
																	                for (int i = 1; i < n - 1; i++) {
																				                for (int j = 1; j < n - 1; j++) {
																							                    u[i][j] = up[i][j];
																									                    }
																						            }
																			        }
												    }
										    printf("%d\n", itter);
										        printf("%e", error);
											    return 0;
}
