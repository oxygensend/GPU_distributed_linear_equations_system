# GPU_linear_equations_system


Obliczenia rozproszone na platformach graficznych (GPU). Celem projektu jest stworzenie systemu służącego do rozwiązania układu liniowego równań z wykorzystaniem GPU w systemie rozproszonym. Na system muszą składać się minimum trzy niezależne maszyny komunikujące się poprzez sieć lokalną. W przypadku gdy jedno z urządzeń przestanie działać, to pozostałe powinny móc kontynuować jego pracę.
	
	
## Instalacja
### Serwer
Serwer obsługujący zdalne połączenie pomiędzy jednostkami obliczeniowymi przygotowany jest w Pythonie. Aby go uruchomić należy wykonać poniższe polecenia a także otworzyć porty :300 oraz :1111 na zewnętrzne połączenia TCP.
```
git clone https://github.com/oxygensend/GPU_distributed_linear_equations_system.git
cd GPU_distributed_linear_equations_system/
cd server
pip install flask simple_websocket_server numpy
python index.py
```
