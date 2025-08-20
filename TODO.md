# Li khasah chi haja i ktbha hnaya w ila saybha y checkiha layr7m lwalidin

--> olaaroub
- [x] mse7 sessions ila client sift lik logout
- [x] 9ad logic dial send response bach l videos w responses lkbar itsifto kamlin (sifet chwi b chwi)
- [ ] normalizi path (127.0.0.1:8080/../ makhashach doz bach mayms7ch lia chi 7aja)
- [x] handli max body size
- [ ]

--> iahamdan
- [ ] 9ad DELETE
- [ ] 9ad l code dialk (ms7 dkchi li matatkhdmch bih w7yed comments ..)
- [ ] 9ad throw logic( hadi khli ta nsaybha m3ak mni nrj3 blama dirha nta bo7dk 7it ba9i ghanchangiw logic)
- [ ] tistit bwahd script ou tl3 lik had error "vector::_M_range_check: __n (which is 0) >= this->size() (which is 0)" script smito "Post_filed_test.py" -> kifach tranih -> dir python3 Post_filed_test.py
- [ ]

--> ohammou
- [x] fixi https (the HTTPS request have a special character in begin called TLS ClientHello, he send it to start a secure connection, bhal ila kaysift wahd lkey to encrypt the request and response base on it, ana ach kandir mli kayban liya hadok lcharacters li kaybda bihom TLS ClientHello kanclose lconnection)
- [x -> but anzid ntesti] testi testi testi toma testi layrdi 3lik (makan3rfch ntesti mzn)
- [in progress] 9ad tanta code dialk ( 7yed implementation mn header.hpp w7yed functions li makhdamch bihom)
- [x 9adit dyawli m3a signals khaskom ntoma t9ado dyalkom] leaks(khlihom homa lkhrin) the only allocation I do is servers-client allocation
- [ ] path normalization
- [x] move the implemontation to .cpp
- [x] ila request fatet max body size anwli nsift 418
- [x] 3ndi mochekil fe timeout hitach kantime outi ila t3tlat respons
- [x] nhyd function li kantkonverti max bodysize
- [x] move ParsError to utils file