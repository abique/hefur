#ifndef HEFUR_TEMPLATE_FACTORY_HH
# define HEFUR_TEMPLATE_FACTORY_HH

# include <mimosa/singleton.hh>
# include <mimosa/tpl/cache.hh>

namespace hefur
{
  /**
   * This is factory which caches parsed html template files.
   */
  class TemplateFactory : public m::Singleton<TemplateFactory>
  {
  public:
    /**
     * Fetches the template designed by path.
     */
    mt::Template::ConstPtr create(const char *path);

  private:
    mt::Cache cache_;
  };
}

#endif /* !HEFUR_TEMPLATE_FACTORY_HH */
