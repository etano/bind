#include "utils/bind.hpp"
#define __a_ceil(x) (((double)x-(int)x) == 0 ? (int)x : (int)x+1)
#define IB 256

inline size_t __a_mod(size_t size, size_t tile){
    size_t m = size % tile;
    if(m == 0) m = tile;
    return m;
}

template<typename T> class stencil;
template<typename T> class border;

namespace detail { 
    using namespace bind;

    // Dirichlet boundaries; central differences in space, forward Euler in time:
    // u[i,j]' = u[i,j] + dt*D*(u(i,j+1) + u(i,j-1) + u(i+1,j) + u(i-1,j) - 4*u(i,j)) / (dr * dr)

    template<typename T>
    void partial_init_value(volatile block<T>& a, T& value, 
                            double& posi, double& posj, 
                            double& dr, double& bound)
    {
        block<T>& av = const_cast<block<T>&>(a);
        size_t m = av.num_rows();
        size_t n = av.num_cols();
        T* a_ = av.data(); 
        memset(a_, 0, bind::extent(av)); 
        for(size_t i = 0; i < m; ++i)
        for(size_t j = 0; j < n; ++j){
           if(std::fabs(posi+i*dr) < bound && 
              std::fabs(posj+j*dr) < bound)
                a_[i + j*m] = value;
        }
    }
    template<typename T>
    void reduce(const block<T>& a,const block<T>& left,
                                  const block<T>& right,
                                  const block<T>& top,
                                  const block<T>& bottom,
                                  double*& res)
    {
        const T* a_ = a.data();
        const T* l  = left.data();
        const T* r  = right.data();
        const T* t  = top.data();
        const T* b  = bottom.data();

        double summ = 0.;
        size_t m = a.num_rows();
        size_t n = a.num_cols();

        for(int j = 0; j < n+2; j++) summ += t[j];
        for(int i = 0; i < m; i++){
            summ += l[i];
            for(int j = 0; j < n; j++) summ += a_[i+j*m];
            summ += r[i];
        }
        for(int j = 0; j < n+2; j++) summ += b[j];
        *res = summ;
    }
    template<typename T>
    void reduce_moment(const block<T>& a, const block<T>& left,
                                          const block<T>& right,
                                          const block<T>& top,
                                          const block<T>& bottom,
                                          double& x, double& y, 
                                          double& dr, double*& res)
    {

        const T* a_ = a.data();
        const T* l  = left.data();
        const T* r  = right.data();
        const T* t  = top.data();
        const T* b  = bottom.data();

        double summ = 0.;
        size_t m = a.num_rows();
        size_t n = a.num_cols();

        double x_, y_;
        for(int j = 0; j < n+2; j++){ x_ = x + j*dr; y_ = y; summ += t[j]*(x_*x_+y_*y_); }
        for(int i = 0; i < m; i++){
            x_ = x; y_ = y  + (i+1)*dr;
            summ += l[i]*(x_*x_+y_*y_);
            for(int j = 0; j < n; j++){ x_ = x + (j+1)*dr; summ += a_[i+j*m]*(x_*x_+y_*y_); }
            x_ = x + (n+1)*dr;
            summ += r[i]*(x_*x_+y_*y_);
        }
        y_ = y + (m+1)*dr;
        for(int j = 0; j < n+2; j++){ x_ = x + j*dr; summ += b[j]*(x_*x_+y_*y_); }
        *res = summ;
    }
    template<typename T>
    void evolve(volatile stencil<T>& s_, const stencil<T>& s,
                                         const border<T>& top, 
                                         const border<T>& right,
                                         const border<T>& bottom,
                                         const border<T>& left, 
                                         double& fac)
    {
        size_t ind;
        size_t m = s.num_rows()-2;
        size_t n = s.num_cols()-2;
        T* u_ = s_.data();
        const T* u  = s.data();
        const T* t  = top.data();
        const T* r  = right.data();
        const T* b  = bottom.data();
        const T* l  = left.data();

        ind = (n-1)*m;     u_[ind] = u[ind] + fac*(u[ind+1] + t[n]     + r[0]     + u[ind-m] - 4*u[ind]);
        ind = m-1;         u_[ind] = u[ind] + fac*(b[1]     + u[ind-1] + u[ind+m] + l[ind]   - 4*u[ind]);
        ind = 0;           u_[ind] = u[ind] + fac*(u[ind+1] + t[1]     + u[ind+m] + l[0]     - 4*u[ind]);
        ind = n*m-1;       u_[ind] = u[ind] + fac*(b[n]     + u[ind-1] + r[m-1]   + u[ind-m] - 4*u[ind]);
        for(ind = 1; ind < m-1; ind++){
                           u_[ind] = u[ind] + fac*(u[ind+1] + u[ind-1] + u[ind+m] + l[ind]   - 4*u[ind]);
        }
        for(int j = 1; j < n-1; j++){
        ind = j*m;         u_[ind] = u[ind] + fac*(u[ind+1] + t[j+1]   + u[ind+m] + u[ind-m] - 4*u[ind]);
        }
        for(int j = 1; j < n-1; j++){
        ind = j*m + m-1;   u_[ind] = u[ind] + fac*(b[j+1]   + u[ind-1] + u[ind+m] + u[ind-m] - 4*u[ind]);
        }
        for(int i = 1; i < m-1; i++){
        ind = i + (n-1)*m; u_[ind] = u[ind] + fac*(u[ind+1] + u[ind-1] + r[i]     + u[ind-m] - 4*u[ind]);
        }
        for(int j = 1; j < n-1; j++){
            ind = j*m;

            for(int i = 0; i < m-2; i++){
                ind++;     u_[ind] = u[ind] + fac*(u[ind+1] + u[ind-1] + u[ind+m] + u[ind-m] - 4*u[ind]);
            }

        }
    }

    template<typename T>
    void contract_border_top(volatile border<T>& top_, const border<T>& left,
                                                       const border<T>& top,
                                                       const border<T>& right,
                                                       const stencil<T>& s,
                                                       const border<T>& ln_top,
                                                       const border<T>& tn_bottom,
                                                       const border<T>& rn_top,
                                                       double& fac)
    {
        size_t n = const_cast<border<T>&>(top_).num_cols()-1;
        size_t m = s.num_rows()-2;
        T* t_ = top_.data();
        const T* t  = top.data();
        const T* l  = left.data();
        const T* r  = right.data();
        const T* x  = s.data();
        const T* lt = ln_top.data();
        const T* tb = tn_bottom.data();
        const T* rt = rn_top.data();
                                   t_[0] = t[0] + fac*(t[1] + lt[ln_top.num_cols()-1] + l[0] + tb[0] - 4*t[0]);
        for(int j = 1; j < n; j++) t_[j] = t[j] + fac*(t[j+1] + t[j-1] + x[(j-1)*m] + tb[j] - 4*t[j]);
                                   t_[n] = t[n] + fac*(rt[0] + t[n-1] + r[0] + tb[n] - 4*t[n]);
    }

    template<typename T>
    void contract_border_bottom(volatile border<T>& bottom_, const border<T>& left, 
                                                             const border<T>& bottom, 
                                                             const border<T>& right,
                                                             const stencil<T>& s,
                                                             const border<T>& ln_bottom, 
                                                             const border<T>& bn_top,
                                                             const border<T>& rn_bottom,
                                                             double& fac)
    {
        size_t n = const_cast<border<T>&>(bottom_).num_cols()-1;
        size_t m = s.num_rows()-2;
        T* b_ = bottom_.data();
        const T* b  = bottom.data();
        const T* l  = left.data();
        const T* r  = right.data();
        const T* x  = s.data();
        const T* lb = ln_bottom.data();
        const T* bt = bn_top.data();
        const T* rb = rn_bottom.data();

                                   b_[0] = b[0] + fac*(b[1] + lb[ln_bottom.num_cols()-1] + bt[0] + l[m-1] - 4*b[0]);
        for(int j = 1; j < n; j++) b_[j] = b[j] + fac*(b[j+1] + b[j-1] + bt[j] + x[j*m-1] - 4*b[j]);
                                   b_[n] = b[n] + fac*(rb[0] + b[n-1] + bt[n] + r[m-1] - 4*b[n]);
    }

    template<typename T>
    void contract_border_left(volatile border<T>& left_, const border<T>& top, 
                                                         const border<T>& left, 
                                                         const border<T>& bottom,
                                                         const stencil<T>& s,
                                                         const border<T>& ln_right,
                                                         double& fac)
    {
        size_t m = const_cast<border<T>&>(left_).num_rows()-1;
        T* l_ = left_.data();
        const T* l  = left.data();
        const T* t  = top.data();
        const T* b  = bottom.data();
        const T* x  = s.data();
        const T* lr = ln_right.data();

                                   l_[0] = l[0] + fac*(lr[0] + x[0] + t[0] + l[1] - 4*l[0]);
        for(int i = 1; i < m; i++) l_[i] = l[i] + fac*(lr[i] + x[i] + l[i-1] + l[i+1] - 4*l[i]);
                                   l_[m] = l[m] + fac*(lr[m] + x[m] + l[m-1] + b[0] - 4*l[m]);
    }

    template<typename T>
    void contract_border_right(volatile border<T>& right_, const border<T>& top,
                                                           const border<T>& right,
                                                           const border<T>& bottom,
                                                           const stencil<T>&  s,
                                                           const border<T>& rn_left,
                                                           double& fac)
    {
        size_t m = const_cast<border<T>&>(right_).num_rows()-1;
        size_t n = s.num_cols()-2;
        T* r_ = right_.data();
        const T* r  = right.data();
        const T* t  = top.data();
        const T* b  = bottom.data();
        const T* x  = s.data() + (m+1)*(n-1);
        const T* rl = rn_left.data();

                                   r_[0] = r[0] + fac*(x[0] + rl[0] + t[n+1] + r[1] - 4*r[0]);
        for(int i = 1; i < m; i++) r_[i] = r[i] + fac*(x[i] + rl[i] + r[i-1] + r[i+1] - 4*r[i]);
                                   r_[m] = r[m] + fac*(x[m] + rl[m] + r[m-1] + b[n+1] - 4*r[m]);
    } 

}

template<typename T>
void partial_init(bind::block<T>& b, T value, double posi, double posj, double dr, double bound){
    bind::cpu(detail::partial_init_value<T>, b, value, posi, posj, dr, bound);
}

template <typename T>
class border : public bind::block<T>{
public:
    border(size_t m, size_t n) : bind::block<T>(m, n) {}
};

template <typename T>
class stencil : public bind::block<T> {
public:
    class frame {
    public:
        frame(size_t m, size_t n){
            top    = new border<T>(1,n);
            right  = new border<T>(m-2,1);
            bottom = new border<T>(1,n);
            left   = new border<T>(m-2,1);
        }
        border<T>* top;
        border<T>* right;
        border<T>* bottom;
        border<T>* left;
    };
    void fill(T value){
        this->init(value);
        top().init(value);
        right().init(value);
        bottom().init(value);
        left().init(value);
    }
    border<T>& top()    const { return *f->top;    }
    border<T>& bottom() const { return *f->bottom; }
    border<T>& left()   const { return *f->left;   }
    border<T>& right()  const { return *f->right;  }

    void print(){
        for(int j = 0; j < IB; j++) printf("%.2f ", bind::load(top())(j,0));
        printf("\n");
        for(int i = 0; i < IB-2; i++){
            printf("%.2f ", bind::load(left())(i,0));
            for(int j = 0; j < IB-2; j++){
                printf("%.2f ", bind::load(*this)(i,j));
            }
            printf("%.2f ", bind::load(right())(i,0));
            printf("\n");
        }
        for(int j = 0; j < IB; j++) printf("%.2f ", bind::load(bottom())(j,0));
        printf("\n");
    }

    void partial_init(T value, double posi, double posj, double dr, double bound){
        ::partial_init(top(),    value, posi,                   posj,                   dr, bound);
        ::partial_init(right(),  value, posi+dr,                posj+(num_cols()-1)*dr, dr, bound);
        ::partial_init(bottom(), value, posi+(num_rows()-1)*dr, posj,                   dr, bound);
        ::partial_init(left(),   value, posi+dr,                posj,                   dr, bound);
        ::partial_init(*this,    value, posi + dr,              posj + dr,              dr, bound);
    }
    size_t num_rows() const { return bind::block<T>::num_rows()+2; }
    size_t num_cols() const { return bind::block<T>::num_cols()+2; }
                    
    void evolve_from(const stencil& s, double fac){
        bind::cpu(detail::evolve<T>, *this, s, s.top(), s.right(), s.bottom(), s.left(), fac);
    }
    void contract(const stencil& s, const stencil& top, const stencil& right, const stencil& bottom, const stencil& left, double fac){
        bind::cpu(detail::contract_border_top<T>   , this->top(),    s.left(), s.top(),    s.right(),  s, left.top(),    top.bottom(), right.top(),    fac);
        bind::cpu(detail::contract_border_bottom<T>, this->bottom(), s.left(), s.bottom(), s.right(),  s, left.bottom(), bottom.top(), right.bottom(), fac);
        bind::cpu(detail::contract_border_right<T> , this->right(),  s.top(),  s.right(),  s.bottom(), s, right.left(),  fac);
        bind::cpu(detail::contract_border_left<T>  , this->left(),   s.top(),  s.left(),   s.bottom(), s, left.right(),  fac);
    }
    double size(){
        double* res = new double(0.);
        bind::cpu(detail::reduce<T>, *this, left(), right(), top(), bottom(), res);
        bind::sync();
        double resv = *res; delete res;
        return resv;
    }
    double moment(double x, double y, double dr){
        double* res = new double(0.);
        bind::cpu(detail::reduce_moment<T>, *this, left(), right(), top(), bottom(), x, y, dr, res);
        bind::sync();
        double resv = *res; delete res;
        return resv;
    }
    stencil(size_t m, size_t n) : bind::block<T>(m-2, n-2) { f = new frame(m, n); }
    frame* f;
};


class Diffusion2D {
    typedef double value_type;
    typedef stencil<double> stencil_t;
    public:

        Diffusion2D(double D, double rmax, double rmin, size_t N):
        D(D), rmax(rmax), rmin(rmin), N(N), null_stencil(IB, IB),
        time(0)
        {
            // process grid - manual for now //
            int n = bind::nodes::size();
            if(n == 2){
                np = 1;
                nq = 2;
            }else if(n == 4){
                np = 1;
                nq = 4;
            }else if(n == 6){
                np = 1;
                nq = 6;
            }else if(n == 12){
                np = 3;
                nq = 4;
            }else{
                np = 1;
                nq = n;
            }

            dr = (rmax - rmin) / (N - 1); // real space grid spacing
            dt = dr * dr / (6 * D);       // dt < dx*dx / (4*D) for stability
            fac = dt * D / (dr * dr);     // stencil factor
           
            // Bind grid
            mt = nt = __a_ceil(N/IB);
            int tailn = __a_mod(N,IB);
            int tailm = __a_mod(N,IB);
            grid.reserve(mt*nt);
            grid_mirror.reserve(mt*nt);
            
            for(int j = 1; j < nt; j++){
                for(int i = 1; i < mt; i++){
                    grid.push_back(new stencil_t(IB, IB)); grid_mirror.push_back(new stencil_t(IB, IB));
                }
                grid.push_back(new stencil_t(tailm, IB)); grid_mirror.push_back(new stencil_t(tailm, IB));
            }
            {
                for(int i = 1; i < mt; i++){
                    grid.push_back(new stencil_t(IB, tailn)); grid_mirror.push_back(new stencil_t(IB, tailn));
                }
                grid.push_back(new stencil_t(tailm, tailn)); grid_mirror.push_back(new stencil_t(tailm, tailn));
            }
            null_stencil.fill(0.0);

            //initialize grid density(x,y,t=0)
            double bound = 1./2;
            double value = 1.0;
            double posi, posj;

            for(size_t i = 0; i < mt; ++i)
            for(size_t j = 0; j < nt; ++j){
                bind::node select(bind::nodes::begin()+get_rank(i,j));
                get(i,j).partial_init(value, i*IB*dr+rmin, j*IB*dr+rmin, dr, bound);
            }
        }

        int get_rank(int i, int j){
            int fnp = nt / np;
            int fnq = mt / nq;
            int pj = j >= fnp*np ? j % np : j / fnp;
            int pi = i >= fnq*nq ? i % nq : i / fnq;
            return (pj + pi*np);
        }

        void print(){
            get(0,0).print();
        }

        double get_size(){
            double sum = 0;
            for(int i = 0; i < mt; i++)
            for(int j = 0; j < nt; j++)
            sum += get(i,j).size();
            return dr*dr*sum;
        }

        double get_moment(){
            double sum = 0;
            for(size_t i = 0; i < mt; ++i)
            for(size_t j = 0; j < nt; ++j)
            sum += get(i,j).moment(j*IB*dr+rmin, i*IB*dr+rmin, dr); 
            return dr*dr*sum;
        }

        stencil_t& get(int i, int j){
            if(i >= mt || j >= nt || i < 0 || j < 0) return null_stencil;
            return *grid[i+j*mt];
        }

        void propagate_density(){ 
            for(int i = 0; i < mt; i++){
                for(int j = 0; j < nt; j++){
                    bind::node select(bind::nodes::begin()+get_rank(i,j));
                    grid_mirror[i+j*mt]->evolve_from(get(i,j), fac);
                    grid_mirror[i+j*mt]->contract(get(i,j), get(i-1,j), get(i,j+1), get(i+1,j), get(i,j-1), fac);
                }
            }
            std::swap(grid,grid_mirror);
            time += dt;
        }
        // out is {0-N}*dr+rmin , {0-N}*dr+rmin -> rho[{0-N}*N+{0-N}]
    private:
        std::vector<stencil_t*> grid;
        std::vector<stencil_t*> grid_mirror;
        stencil_t null_stencil;
        size_t mt, nt;
        int np;
        int nq;
    private:
        const double D, rmax, rmin;
        const size_t N;
        double dr, dt, fac;
        double time;
};

int main(int argc, char* argv[]){
    double D = 1;
    double rmax = 1;
    double rmin = -1;
    size_t N = 1 << std::stoul(argv[1]);
    std::cout << "Parallel: " << bind::num_procs() << " procs x "
                              << bind::num_threads() << " threads\n"; 
    std::cout << "Domain: " << N << "\n";
    size_t max_steps = 40;
    Diffusion2D task(D, rmax, rmin, N);

    bind::timer time("execution"); time.begin();
    for(size_t steps = 0; steps < max_steps; ++steps){
        task.propagate_density();
    }
    time.end();
    {
        bind::node select(bind::nodes::begin());
        std::cout << "getting results... ";
        std::cout << task.get_size() << '\t' << task.get_moment() << std::endl;
    }
    return 0;
}
